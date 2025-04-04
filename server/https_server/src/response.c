#include "../include/response.h"
#include "../include/utils.h"

#include <stdlib.h>   // for malloc, free
#include <string.h>   // for strcpy, strlen, memcpy
#include <stdio.h>    // for snprintf
#include <sys/stat.h> // for fstat
#include <fcntl.h>    // for open
#include <unistd.h>   // for read, close
#include <regex.h>    // for regex functions
#include <sys/socket.h> // for send, recv

void build_http_response(const char *file_name, 
                        const char *file_ext, 
                        char *response, 
                        size_t *response_len) {
    // Try to find the file case-insensitive
    char *actual_file_name = get_file_case_insensitive(file_name);
    if (!actual_file_name) {
        send_404_response(response, response_len);
        return;
    }

    const char *mime_type = get_mime_type(file_ext);
    char *header = (char *)malloc(BUFFER_SIZE * sizeof(char));
    if (!header) {
        send_500_response(response, response_len);
        free(actual_file_name);
        return;
    }

    snprintf(header, BUFFER_SIZE,
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Connection: close\r\n"
             "\r\n",
             mime_type);

    int file_fd = open(actual_file_name, O_RDONLY);
    if (file_fd == -1) {
        send_404_response(response, response_len);
        free(header);
        free(actual_file_name);
        return;
    }

    struct stat file_stat;
    if (fstat(file_fd, &file_stat) < 0) {
        send_500_response(response, response_len);
        free(header);
        free(actual_file_name);
        close(file_fd);
        return;
    }

    *response_len = 0;
    memcpy(response, header, strlen(header));
    *response_len += strlen(header);

    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, response + *response_len, 
                            BUFFER_SIZE - *response_len)) > 0) {
        *response_len += bytes_read;
    }

    free(header);
    free(actual_file_name);
    close(file_fd);
}

void send_404_response(char *response, size_t *response_len) {
    snprintf(response, BUFFER_SIZE,
             "HTTP/1.1 404 Not Found\r\n"
             "Content-Type: text/plain\r\n"
             "Connection: close\r\n"
             "\r\n"
             "404 Not Found");
    *response_len = strlen(response);
}

void send_500_response(char *response, size_t *response_len) {
    snprintf(response, BUFFER_SIZE,
             "HTTP/1.1 500 Internal Server Error\r\n"
             "Content-Type: text/plain\r\n"
             "Connection: close\r\n"
             "\r\n"
             "500 Internal Server Error");
    *response_len = strlen(response);
}

void handle_client(int client_fd) {
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
    if (!buffer) {
        const char *resp = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
        send(client_fd, resp, strlen(resp), 0);
        close(client_fd);
        return;
    }

    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0) {
        regex_t regex;
        regcomp(&regex, "^GET /([^ ]*) HTTP/1", REG_EXTENDED);
        regmatch_t matches[2];

        if (regexec(&regex, buffer, 2, matches, 0) == 0) {
            buffer[matches[1].rm_eo] = '\0';
            const char *url_encoded_file_name = buffer + matches[1].rm_so;
            char *file_name = url_decode(url_encoded_file_name);

            if (file_name) {
                char file_ext[32];
                strcpy(file_ext, get_file_extension(file_name));

                char *response = (char *)malloc(BUFFER_SIZE * 2 * sizeof(char));
                if (response) {
                    size_t response_len;
                    build_http_response(file_name, file_ext, response, &response_len);
                    send(client_fd, response, response_len, 0);
                    free(response);
                }
                free(file_name);
            }
        }
        regfree(&regex);
    }
    free(buffer);
}