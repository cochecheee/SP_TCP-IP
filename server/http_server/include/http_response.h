// http_response.h
#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <stddef.h>
#define BUFFER_SIZE 104857600

typedef struct {
    int status_code;
    char* content_type;
    char* body;
    size_t body_length;
    char* additional_headers;
} HttpResponse;

void build_http_response(const char *file_name, const char *file_ext, char *response, size_t *response_len);
const char *get_mime_type(const char *file_ext);
void send_response(int client_fd, HttpResponse* resp);
HttpResponse* create_response(int status, const char* content_type, const char* body);
void free_response(HttpResponse* resp);

#endif // HTTP_RESPONSE_H