#include "../include/request_handler.h"
#include "../include/response_handler.h"
#include "../include/utils.h"
#include "../include/http_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctype.h>
#include <openssl/ssl.h>

void parse_headers(const char *request, char **headers) {
    printf("[DEBUG] Parsing headers.\n");
    const char *header_end = strstr(request, "\r\n\r\n");
    if (!header_end) {
        *headers = NULL;
        return;
    }

    size_t headers_len = header_end - request;
    *headers = malloc(headers_len + 1);
    if (!*headers) {
        perror("malloc");
        return;
    }
    strncpy(*headers, request, headers_len);
    (*headers)[headers_len] = '\0';
}

void set_cgi_headers(const char *headers) {
    char *headers_copy = strdup(headers);
    if (!headers_copy) {
        perror("strdup");
        return;
    }

    char *line = strtok(headers_copy, "\r\n");
    while (line) {
        if (strstr(line, "HTTP/") == line) {
            line = strtok(NULL, "\r\n");
            continue;
        }

        char *colon = strchr(line, ':');
        if (colon) {
            *colon = '\0';
            char *header_name = line;
            char *header_value = colon + 1;

            while (*header_value == ' ') {
                header_value++;
            }

            char env_name[256];
            snprintf(env_name, sizeof(env_name), "HTTP_%s", header_name);

            for (char *p = env_name; *p; p++) {
                if (*p == '-') {
                    *p = '_';
                }
                *p = toupper(*p);
            }

            setenv(env_name, header_value, 1);
        }

        line = strtok(NULL, "\r\n");
    }

    free(headers_copy);
}

void handle_php_request(int client_fd, const char *file_path, const char *method, const char *body, const char *headers, SSL *ssl, int use_ssl) {
    printf("[DEBUG] Handling PHP request for file: %s\n", file_path);
    setenv("REQUEST_METHOD", method, 1);
    setenv("SCRIPT_FILENAME", file_path, 1);
    setenv("REDIRECT_STATUS", "200", 1);

    const char *query_string = getenv("QUERY_STRING");
    if (query_string) {
        setenv("QUERY_STRING", query_string, 1);
    } else {
        setenv("QUERY_STRING", "", 1);
    }

    if (headers) {
        set_cgi_headers(headers);
    }

    if (body) {
        char content_length[32];
        snprintf(content_length, sizeof(content_length), "%zu", strlen(body));
        setenv("CONTENT_LENGTH", content_length, 1);
        setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", 1);
    } else {
        unsetenv("CONTENT_LENGTH");
        unsetenv("CONTENT_TYPE");
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        send_response(client_fd, "500 Internal Server Error", "text/plain", "500 Internal Server Error", ssl, use_ssl);
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        send_response(client_fd, "500 Internal Server Error", "text/plain", "500 Internal Server Error", ssl, use_ssl);
        return;
    }

    if (pid == 0) { // Child process
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (body) {
            int body_pipe[2];
            if (pipe(body_pipe) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
            write(body_pipe[1], body, strlen(body));
            close(body_pipe[1]);
            dup2(body_pipe[0], STDIN_FILENO);
            close(body_pipe[0]);
        }

        execlp("php-cgi", "php-cgi", NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else { // Parent process
        close(pipefd[1]);
        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;
        char *response = malloc(BUFFER_SIZE * 2);
        size_t response_len = 0;

        if (!response) {
            perror("malloc");
            close(pipefd[0]);
            send_response(client_fd, "500 Internal Server Error", "text/plain", "500 Internal Server Error", ssl, use_ssl);
            return;
        }

        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            if (response_len + bytes_read >= BUFFER_SIZE * 2) {
                char *new_response = realloc(response, response_len + bytes_read + 1);
                if (!new_response) {
                    free(response);
                    close(pipefd[0]);
                    send_response(client_fd, "500 Internal Server Error", "text/plain", "500 Internal Server Error", ssl, use_ssl);
                    return;
                }
                response = new_response;
            }
            memcpy(response + response_len, buffer, bytes_read);
            response_len += bytes_read;
        }

        close(pipefd[0]);
        waitpid(pid, NULL, 0);

        // Đảm bảo response kết thúc bằng null terminator
        if (response_len < BUFFER_SIZE * 2) {
            response[response_len] = '\0';
        } else {
            response[BUFFER_SIZE * 2 - 1] = '\0';
        }

        char *header_end = strstr(response, "\r\n\r\n");
        if (header_end) {
            *header_end = '\0';
            char *headers_part = response;
            char *body_part = header_end + 4;
            size_t body_length = response_len - (body_part - response);

            char *status_header = strstr(headers_part, "Status:");
            char status_line[64] = "HTTP/1.1 200 OK";
            
            if (status_header) {
                int status_code = atoi(status_header + 8);
                snprintf(status_line, sizeof(status_line), "HTTP/1.1 %d", status_code);
            }

            // Xử lý phần header
            char *http_headers = malloc(strlen(headers_part) + strlen(status_line) + 16);
            if (!http_headers) {
                perror("malloc");
                free(response);
                send_response(client_fd, "500 Internal Server Error", "text/plain", "500 Internal Server Error", ssl, use_ssl);
                return;
            }
            
            int headers_len = snprintf(http_headers, strlen(headers_part) + strlen(status_line) + 16,
                         "%s\r\n%s\r\n\r\n", status_line, headers_part);

            // Gửi header và body riêng biệt để tránh vấn đề với null terminator
            if (use_ssl) {
                SSL_write(ssl, http_headers, headers_len);
                if (body_length > 0) {
                    SSL_write(ssl, body_part, body_length);
                }
            } else {
                write(client_fd, http_headers, headers_len);
                if (body_length > 0) {
                    write(client_fd, body_part, body_length);
                }
            }
            
            free(http_headers);
        } else {
            // Không tìm thấy header, gửi toàn bộ response
            if (use_ssl) {
                SSL_write(ssl, response, response_len);
            } else {
                write(client_fd, response, response_len);
            }
        }

        free(response);
        printf("[DEBUG] Response sent to client.\n");
    }
}

void handle_request(int client_fd, const char *request, SSL *ssl, int use_ssl) {
    char method[16], path[1024], protocol[16];
    if (sscanf(request, "%15s %1023s %15s", method, path, protocol) != 3) {
        send_response(client_fd, "400 Bad Request", "text/plain", "400 Bad Request", ssl, use_ssl);
        return;
    }

    char *query_string = strchr(path, '?');
    if (query_string) {
        *query_string = '\0';
        query_string++;
    } else {
        query_string = "";
    }

    setenv("QUERY_STRING", query_string, 1);

    char *headers = NULL;
    parse_headers(request, &headers);

    char *body = NULL;
    if (strcmp(method, "POST") == 0 || strcmp(method, "PUT") == 0) {
        const char *body_start = strstr(request, "\r\n\r\n");
        if (body_start) {
            body_start += 4;
            body = strdup(body_start);
        }
    }

    if (strcmp(path, "/") == 0) {
        char index_php_path[2048];
        snprintf(index_php_path, sizeof(index_php_path), "%s/index.php", DOCUMENT_ROOT);
        if (access(index_php_path, F_OK) == 0) {
            strcpy(path, "/index.php");
        } else {
            strcpy(path, "/index.html");
        }
    }

    char file_path[2048];
    snprintf(file_path, sizeof(file_path), "%s%s", DOCUMENT_ROOT, path);

    if (access(file_path, F_OK) == -1) {
        send_response(client_fd, "404 Not Found", "text/plain", "404 Not Found", ssl, use_ssl);
        if (body) free(body);
        if (headers) free(headers);
        return;
    }

    const char *file_ext = strrchr(path, '.') ? strrchr(path, '.') + 1 : "";
    if (strcasecmp(file_ext, "php") == 0) {
        handle_php_request(client_fd, file_path, method, body, headers, ssl, use_ssl);
    } else {
        if (strcmp(method, "GET") == 0) {
            int file_fd = open(file_path, O_RDONLY);
            if (file_fd < 0) {
                send_response(client_fd, "404 Not Found", "text/plain", "404 Not Found", ssl, use_ssl);
                if (body) free(body);
                if (headers) free(headers);
                return;
            }

            struct stat file_stat;
            fstat(file_fd, &file_stat);
            off_t file_size = file_stat.st_size;
            const char *mime_type = get_mime_type(file_ext);

            char header[BUFFER_SIZE];
            snprintf(header, sizeof(header),
                     "HTTP/1.1 200 OK\r\n"
                     "Content-Type: %s\r\n"
                     "Content-Length: %ld\r\n"
                     "\r\n",
                     mime_type, file_size);

            if (use_ssl) {
                SSL_write(ssl, header, strlen(header));
            } else {
                write(client_fd, header, strlen(header));
            }

            char buffer[BUFFER_SIZE];
            ssize_t bytes_read;
            while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0) {
                if (use_ssl) {
                    SSL_write(ssl, buffer, bytes_read);
                } else {
                    write(client_fd, buffer, bytes_read);
                }
            }

            close(file_fd);
        } else {
            send_response(client_fd, "405 Method Not Allowed", "text/plain", "Method Not Allowed", ssl, use_ssl);
        }
    }

    if (body) free(body);
    if (headers) free(headers);
}
