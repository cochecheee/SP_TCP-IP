// http_server.c
#include "../include/http_server.h"
#include "../include/http_request.h"
#include "../include/http_response.h"
#include "../include/file_utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

#define MAX_ROUTES 100
#define MAX_USERS 100

static Route routes[MAX_ROUTES];
static int route_count = 0;
static FILE* log_file = NULL;

typedef struct {
    int id;
    char name[50];
    char email[100];
} User;

static User users[MAX_USERS];
static int user_count = 0;

static void example_handler(int client_fd, HttpRequest* req);

void add_route(const char* path, void (*handler)(int, HttpRequest*)) {
    if (route_count < MAX_ROUTES) {
        routes[route_count].path = path;
        routes[route_count].handler = handler;
        route_count++;
        printf("Registered route: %s\n", path);  // Debug route registration
    }
}

bool authenticate_request(HttpRequest* req) {
    return true;
}

void log_request(HttpRequest* req, const char* client_ip) {
    if (!log_file) {
        log_file = fopen("server.log", "a");
    }
    if (log_file) {
        time_t now;
        time(&now);
        char* method_str[] = {"GET", "POST", "PUT", "DELETE", "HEAD", "UNKNOWN"};
        fprintf(log_file, "[%s] %s %s %s\n", 
                ctime(&now), client_ip, method_str[req->method], req->path);
        fflush(log_file);
    }
}

void serve_file(int client_fd, HttpRequest* req) {
    printf("Serving file: %s\n", req->path);  // Debug file serving
    char* response = malloc(BUFFER_SIZE * 2);
    size_t response_len;
    const char* file_ext = get_file_extension(req->path);
    build_http_response(req->path, file_ext, response, &response_len);
    send(client_fd, response, response_len, 0);
    free(response);
}

static void example_handler(int client_fd, HttpRequest* req) {
    char response[256];
    size_t len = snprintf(response, 256,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n\r\n"
        "{\"message\": \"Hello from API\"}");
    send(client_fd, response, len, 0);
}

void handle_users(int client_fd, HttpRequest* req) {
    printf("Handling /api/users for method %d\n", req->method);  // Debug handler
    HttpResponse* resp = NULL;
    
    switch (req->method) {
        case HTTP_GET: {
            char body[BUFFER_SIZE];
            size_t offset = snprintf(body, BUFFER_SIZE, "[");
            for (int i = 0; i < user_count; i++) {
                offset += snprintf(body + offset, BUFFER_SIZE - offset,
                    "{\"id\":%d,\"name\":\"%s\",\"email\":\"%s\"}%s",
                    users[i].id, users[i].name, users[i].email,
                    (i < user_count - 1) ? "," : "");
            }
            offset += snprintf(body + offset, BUFFER_SIZE - offset, "]");
            resp = create_response(200, "application/json", body);
            break;
        }
        case HTTP_POST: {
            if (req->body && user_count < MAX_USERS) {
                char name[50], email[100];
                if (sscanf(req->body, "{\"name\":\"%49[^\"]\",\"email\":\"%99[^\"]\"}", 
                          name, email) == 2) {
                    users[user_count].id = user_count + 1;
                    strncpy(users[user_count].name, name, 49);
                    strncpy(users[user_count].email, email, 99);
                    user_count++;
                    char body[256];
                    snprintf(body, 256, "{\"message\":\"User created\",\"id\":%d}", 
                            users[user_count-1].id);
                    resp = create_response(201, "application/json", body);
                } else {
                    resp = create_response(400, "application/json", 
                                         "{\"error\":\"Invalid request body\"}");
                }
            } else {
                resp = create_response(400, "application/json", 
                                     "{\"error\":\"No body or max users reached\"}");
            }
            break;
        }
        case HTTP_PUT: {
            int id = 0;
            if (sscanf(req->path, "/api/users/%d", &id) == 1) {
                for (int i = 0; i < user_count; i++) {
                    if (users[i].id == id) {
                        if (req->body) {
                            char name[50], email[100];
                            if (sscanf(req->body, "{\"name\":\"%49[^\"]\",\"email\":\"%99[^\"]\"}", 
                                      name, email) == 2) {
                                strncpy(users[i].name, name, 49);
                                strncpy(users[i].email, email, 99);
                                resp = create_response(200, "application/json", 
                                                     "{\"message\":\"User updated\"}");
                            } else {
                                resp = create_response(400, "application/json", 
                                                     "{\"error\":\"Invalid request body\"}");
                            }
                        }
                        break;
                    }
                }
                if (!resp) {
                    resp = create_response(404, "application/json", 
                                         "{\"error\":\"User not found\"}");
                }
            } else {
                resp = create_response(400, "application/json", 
                                     "{\"error\":\"Invalid user ID\"}");
            }
            break;
        }
        case HTTP_DELETE: {
            int id = 0;
            if (sscanf(req->path, "/api/users/%d", &id) == 1) {
                for (int i = 0; i < user_count; i++) {
                    if (users[i].id == id) {
                        for (int j = i; j < user_count - 1; j++) {
                            users[j] = users[j + 1];
                        }
                        user_count--;
                        resp = create_response(200, "application/json", 
                                             "{\"message\":\"User deleted\"}");
                        break;
                    }
                }
                if (!resp) {
                    resp = create_response(404, "application/json", 
                                         "{\"error\":\"User not found\"}");
                }
            } else {
                resp = create_response(400, "application/json", 
                                     "{\"error\":\"Invalid user ID\"}");
            }
            break;
        }
        default:
            resp = create_response(405, "application/json", 
                                 "{\"error\":\"Method not allowed\"}");
            break;
    }
    
    send_response(client_fd, resp);
    free_response(resp);
}

void handle_status(int client_fd, HttpRequest* req) {
    if (req->method == HTTP_GET) {
        char body[256];
        snprintf(body, 256, "{\"status\":\"running\",\"users\":%d,\"uptime\":%ld}", 
                user_count, time(NULL));
        HttpResponse* resp = create_response(200, "application/json", body);
        send_response(client_fd, resp);
        free_response(resp);
    } else {
        HttpResponse* resp = create_response(405, "application/json", 
                                           "{\"error\":\"Method not allowed\"}");
        send_response(client_fd, resp);
        free_response(resp);
    }
}

void handle_data(int client_fd, HttpRequest* req) {
    if (req->method == HTTP_POST) {
        if (req->body) {
            char body[BUFFER_SIZE];
            snprintf(body, BUFFER_SIZE, "{\"received\":\"%s\"}", req->body);
            HttpResponse* resp = create_response(200, "application/json", body);
            send_response(client_fd, resp);
            free_response(resp);
        } else {
            HttpResponse* resp = create_response(400, "application/json", 
                                               "{\"error\":\"No data provided\"}");
            send_response(client_fd, resp);
            free_response(resp);
        }
    } else {
        HttpResponse* resp = create_response(405, "application/json", 
                                           "{\"error\":\"Method not allowed\"}");
        send_response(client_fd, resp);
        free_response(resp);
    }
}

void *handle_client(void *arg) {
    int client_fd = *((int *)arg);
    char *buffer = malloc(BUFFER_SIZE);

    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0) {
        HttpRequest* req = parse_http_request(buffer, bytes_received);
        if (req) {
            char client_ip[INET_ADDRSTRLEN];
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            getpeername(client_fd, (struct sockaddr*)&client_addr, &client_len);
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            
            log_request(req, client_ip);
            printf("Request path: %s\n", req->path);  // Debug path
            
            if (!authenticate_request(req)) {
                char response[256];
                size_t len = snprintf(response, 256,
                    "HTTP/1.1 401 Unauthorized\r\n"
                    "Content-Type: text/plain\r\n\r\n"
                    "Unauthorized");
                send(client_fd, response, len, 0);
            } else {
                bool handled = false;
                for (int i = 0; i < route_count; i++) {
                    size_t route_len = strlen(routes[i].path);
                    printf("Checking route: %s against %s\n", routes[i].path, req->path);  // Debug route matching
                    if (strncmp(routes[i].path, req->path, route_len) == 0 &&
                        (req->path[route_len] == '\0' || req->path[route_len] == '/')) {
                        routes[i].handler(client_fd, req);
                        handled = true;
                        break;
                    }
                }
                if (!handled) {
                    printf("No route matched, falling back to file serving\n");
                    serve_file(client_fd, req);
                }
            }
            free_http_request(req);
        }
    }
    
    close(client_fd);
    free(arg);
    free(buffer);
    return NULL;
}

int start_http_server(int port) {
    add_route("/api/example", example_handler);
    add_route("/api/users", handle_users);
    add_route("/api/status", handle_status);
    add_route("/api/data", handle_data);

    int server_fd;
    struct sockaddr_in server_addr;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        return -1;
    }
    
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        close(server_fd);
        return -1;
    }

    printf("Server listening on port %d\n", port);
    
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int *client_fd = malloc(sizeof(int));

        if ((*client_fd = accept(server_fd, 
                               (struct sockaddr *)&client_addr, 
                               &client_addr_len)) < 0) {
            perror("accept failed");
            free(client_fd);
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Client connected: %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, (void *)client_fd) != 0) {
            perror("pthread_create failed");
            close(*client_fd);
            free(client_fd);
            continue;
        }
        pthread_detach(thread_id);
    }

    close(server_fd);
    return 0;
}