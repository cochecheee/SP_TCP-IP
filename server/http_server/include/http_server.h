// http_server.h
#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <stdbool.h>
#include <stddef.h>
#include "../include/http_request.h"  // Include the HttpRequest definition

#define PORT 8080
#define BUFFER_SIZE 104857600

typedef struct {
    const char* path;
    void (*handler)(int client_fd, HttpRequest* req);
} Route;

int start_http_server(int port);
void *handle_client(void *arg);
void add_route(const char* path, void (*handler)(int, HttpRequest*));
bool authenticate_request(HttpRequest* req);
void log_request(HttpRequest* req, const char* client_ip);

// Khai báo các handler cho endpoint
void handle_users(int client_fd, HttpRequest* req);
void handle_status(int client_fd, HttpRequest* req);
void handle_data(int client_fd, HttpRequest* req);

#endif // HTTP_SERVER_H