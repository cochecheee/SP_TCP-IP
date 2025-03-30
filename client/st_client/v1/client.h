#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SIZE 256

void error(const char *msg);
int create_socket();
struct hostent *resolve_host(const char *hostname);
void connect_to_server(int sockfd, struct sockaddr_in *serv_addr, struct hostent *server, int portno);
void communicate_with_server(int sockfd);

#endif
