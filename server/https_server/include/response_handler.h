#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <openssl/ssl.h>

void send_response(int client_fd, const char *status, const char *content_type, const char *body, SSL *ssl, int use_ssl);

#endif
