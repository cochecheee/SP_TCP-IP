#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <openssl/ssl.h> // Thêm dòng này

void handle_request(int client_fd, const char *request, SSL *ssl);
void handle_php_request(int client_fd, const char *file_path, const char *method, const char *body, const char *headers, SSL *ssl);

#endif