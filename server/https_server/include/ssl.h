#ifndef SSL_H
#define SSL_H

#include <openssl/ssl.h>
#include <openssl/err.h>

#define BUFFER_SIZE 104857600

// Function declarations
void ssl_init(void);
void ssl_cleanup(void);
SSL_CTX* create_ssl_context(void);
void configure_ssl_context(SSL_CTX *ctx, const char *cert_path, const char *key_path);
void ssl_handle_client(SSL *ssl);

#endif /* SSL_H */