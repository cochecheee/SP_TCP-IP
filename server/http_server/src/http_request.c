// http_request.c
#include "../include/http_request.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HttpRequest* parse_http_request(const char *request, size_t request_size) {
    HttpRequest* req = malloc(sizeof(HttpRequest));
    if (!req) return NULL;
    
    req->path = NULL;
    req->body = NULL;
    req->body_length = 0;
    
    // Determine method
    if (strncmp(request, "GET ", 4) == 0) req->method = HTTP_GET;
    else if (strncmp(request, "POST ", 5) == 0) req->method = HTTP_POST;
    else if (strncmp(request, "PUT ", 4) == 0) req->method = HTTP_PUT;
    else if (strncmp(request, "DELETE ", 7) == 0) req->method = HTTP_DELETE;
    else if (strncmp(request, "HEAD ", 5) == 0) req->method = HTTP_HEAD;
    else req->method = HTTP_UNKNOWN;

    // Extract path using regex
    regex_t regex;
    regcomp(&regex, "^[A-Z]+ /([^ ]*) HTTP/1", REG_EXTENDED);
    regmatch_t matches[2];

    if (regexec(&regex, request, 2, matches, 0) == 0) {
        size_t url_len = matches[1].rm_eo - matches[1].rm_so;
        char* url_encoded = malloc(url_len + 2);  // +2 for '/' and null terminator
        if (url_encoded) {
            // Ensure path starts with '/'
            url_encoded[0] = '/';
            strncpy(url_encoded + 1, request + matches[1].rm_so, url_len);
            url_encoded[url_len + 1] = '\0';
            req->path = url_decode(url_encoded);
            free(url_encoded);
        }
    }
    regfree(&regex);

    // Extract body for POST/PUT
    if (req->method == HTTP_POST || req->method == HTTP_PUT) {
        const char* body_start = strstr(request, "\r\n\r\n");
        if (body_start) {
            body_start += 4;
            req->body_length = request_size - (body_start - request);
            req->body = malloc(req->body_length + 1);
            if (req->body) {
                memcpy(req->body, body_start, req->body_length);
                req->body[req->body_length] = '\0';
            }
        }
    }

    if (!req->path) {
        req->path = strdup("/www/index.html");  // Default path with '/'
    }
    printf("Parsed path: %s\n", req->path);  // Debug parsed path
    return req;
}

void free_http_request(HttpRequest* request) {
    if (request) {
        if (request->path) free(request->path);
        if (request->body) free(request->body);
        free(request);
    }
}

char *url_decode(const char *src) {
    if (src == NULL) return NULL;
    
    size_t src_len = strlen(src);
    char *decoded = malloc(src_len + 1);
    if (decoded == NULL) return NULL;
    
    size_t decoded_len = 0;
    for (size_t i = 0; i < src_len; i++) {
        if (src[i] == '%' && i + 2 < src_len) {
            int hex_val;
            sscanf(src + i + 1, "%2x", &hex_val);
            decoded[decoded_len++] = hex_val;
            i += 2;
        } else if (src[i] == '+') {
            decoded[decoded_len++] = ' ';
        } else {
            decoded[decoded_len++] = src[i];
        }
    }
    decoded[decoded_len] = '\0';
    
    if (strlen(decoded) == 1 && decoded[0] == '/') {
        free(decoded);
        return strdup("/www/index.html");
    }
    return decoded;
}