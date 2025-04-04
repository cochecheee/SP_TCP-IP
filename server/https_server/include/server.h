#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <stdbool.h>
#include <regex.h>

#define PORT 8080
#define BUFFER_SIZE 104857600
#define MAX_CLIENTS 10

typedef struct {
    int fd;
    struct sockaddr_in address;
} Server;

Server* server_create();
bool server_bind(Server *server, int port);
void server_listen(Server *server);
void server_destroy(Server *server);
void handle_client(int client_fd);

#endif