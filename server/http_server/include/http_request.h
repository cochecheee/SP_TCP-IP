// http_request.h
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_HEAD,
    HTTP_UNKNOWN
} HttpMethod;

typedef struct {
    HttpMethod method;
    char* path;
    char* body;
    size_t body_length;
} HttpRequest;

/**
 * Parse HTTP request and extract information
 */
HttpRequest* parse_http_request(const char *request, size_t request_size);

/**
 * Free HttpRequest structure
 */
void free_http_request(HttpRequest* request);

/**
 * Decode URL encoded string
 */
char *url_decode(const char *src);

#endif // HTTP_REQUEST_H