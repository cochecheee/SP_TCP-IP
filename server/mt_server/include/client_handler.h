#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H
#include <arpa/inet.h>

#define BUFFER_SIZE 1024 
// Structure lưu thông tin client
typedef struct {
    int client_socket;
    struct sockaddr_in client_addr;
} client_info_t;

// Prototype function xử lý client
void *handle_client(void *arg);

#endif
