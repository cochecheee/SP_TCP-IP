#include "../include/response_handler.h"
#include "../include/http_server.h" // Thêm dòng này để sử dụng BUFFER_SIZE
#include <stdio.h>                  // Thêm cho snprintf
#include <string.h>                 // Thêm cho strlen
#include <sys/socket.h>             // Thêm cho send

void send_response(int client_fd, const char *status, const char *content_type, const char *body) {
    char response[BUFFER_SIZE];
    int body_length = strlen(body);

    snprintf(response, sizeof(response),
             "HTTP/1.1 %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %d\r\n"
             "\r\n"
             "%s",
             status, content_type, body_length, body);

    send(client_fd, response, strlen(response), 0);
}
