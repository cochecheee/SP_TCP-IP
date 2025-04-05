#include "../include/response_handler.h"
#include "../include/http_server.h"
#include <stdio.h>
#include <string.h>
#include <openssl/ssl.h>

void send_response(int client_fd, const char *status, const char *content_type, const char *body, SSL *ssl) {
    char response[BUFFER_SIZE];
    int body_length = strlen(body);

    snprintf(response, sizeof(response),
             "HTTP/1.1 %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %d\r\n"
             "\r\n"
             "%s",
             status, content_type, body_length, body);

    SSL_write(ssl, response, strlen(response));
}