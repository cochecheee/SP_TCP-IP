#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server_2.h"

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int create_server_socket(int port) {
    int sockfd;
    struct sockaddr_in serv_addr;

    // Tạo socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    // Cấu hình địa chỉ server
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    // Bind socket với địa chỉ server
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    // Lắng nghe kết nối
    listen(sockfd, 5);

    return sockfd;
}

int accept_connection(int sockfd, struct sockaddr_in *cli_addr, socklen_t *clilen) {
    int newsockfd = accept(sockfd, (struct sockaddr *)cli_addr, clilen);
    if (newsockfd < 0) error("ERROR on accept");
    return newsockfd;
}

int receive_message(int sockfd, char *buffer, int size) {
    bzero(buffer, size);
    int n = read(sockfd, buffer, size - 1);
    if (n < 0) error("ERROR reading from socket");
    return n;
}

void send_message(int sockfd, const char *message) {
    int n = write(sockfd, message, strlen(message));
    if (n < 0) error("ERROR writing to socket");
}
