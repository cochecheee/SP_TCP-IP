#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <netinet/in.h>

void error(const char *msg);
int create_server_socket(int port);
int accept_connection(int sockfd, struct sockaddr_in *cli_addr, socklen_t *clilen);
int receive_message(int sockfd, char *buffer, int size);
void send_message(int sockfd, const char *message);

#endif // SERVER_UTILS_H
