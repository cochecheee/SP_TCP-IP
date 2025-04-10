#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <pthread.h>
#include <netinet/in.h>

#define HTTP_PORT 8080
#define BUFFER_SIZE 4096
#define DOCUMENT_ROOT "./www"

void *handle_client(void *arg);
void start_server();

#endif