#include "../include/ssl.h"
#include <regex.h>     // For regex functions
#include <string.h>    // For strcpy
#include <stdlib.h>    // For malloc/free

void ssl_init() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void ssl_cleanup() {
    EVP_cleanup();
}

SSL_CTX* create_ssl_context() {
    const SSL_METHOD *method = TLS_server_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

void configure_ssl_context(SSL_CTX *ctx, const char *cert_path, const char *key_path) {
    SSL_CTX_set_ecdh_auto(ctx, 1);

    // Set the key and cert
    if (SSL_CTX_use_certificate_file(ctx, cert_path, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, key_path, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

void ssl_handle_client(SSL *ssl) {
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
    if (!buffer) {
        const char *resp = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
        SSL_write(ssl, resp, strlen(resp));
        SSL_free(ssl);
        return;
    }

    int bytes_received = SSL_read(ssl, buffer, BUFFER_SIZE);
    if (bytes_received > 0) {
        regex_t regex;
        regcomp(&regex, "^GET /([^ ]*) HTTP/1", REG_EXTENDED);
        regmatch_t matches[2];

        if (regexec(&regex, buffer, 2, matches, 0) == 0) {
            buffer[matches[1].rm_eo] = '\0';
            const char *url_encoded_file_name = buffer + matches[1].rm_so;
            char *file_name = url_decode(url_encoded_file_name);

            if (file_name) {
                char file_ext[32];
                strcpy(file_ext, get_file_extension(file_name));

                char *response = (char *)malloc(BUFFER_SIZE * 2 * sizeof(char));
                if (response) {
                    size_t response_len;
                    build_http_response(file_name, file_ext, response, &response_len);
                    SSL_write(ssl, response, response_len);
                    free(response);
                }
                free(file_name);
            }
        }
        regfree(&regex);
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    free(buffer);
}