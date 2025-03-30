#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 256
#define PORT 8081

void error(const char *msg);
int create_socket();
void bind_socket(int sockfd, struct sockaddr_in *serv_addr);
int accept_connection(int sockfd, struct sockaddr_in *cli_addr, socklen_t *clilen);
void handle_client(int newsockfd);

#endif
