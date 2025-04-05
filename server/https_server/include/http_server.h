#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <pthread.h>
#include <netinet/in.h>
#include <openssl/ssl.h>   // Thêm OpenSSL
#include <openssl/err.h>   // Thêm OpenSSL

#define PORT 9443       
#define BUFFER_SIZE 4096
#define DOCUMENT_ROOT "./www"

void *handle_client(void *arg);
void start_server();

#endif