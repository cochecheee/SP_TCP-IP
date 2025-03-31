#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "client.h"

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int connect_to_server(const char *server_ip, int port) {
    int sockfd;
    struct sockaddr_in serv_addr;

    // Tạo socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    // Cấu hình địa chỉ server
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        error("ERROR invalid address or address not supported");
    }

    // Kết nối đến server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    return sockfd;
}

void send_message(int sockfd, const char *message) {
    int n = write(sockfd, message, strlen(message));
    if (n < 0) 
        error("ERROR writing to socket");
}

void receive_message(int sockfd, char *buffer, int size) {
    bzero(buffer, size);
    int n = read(sockfd, buffer, size - 1);
    if (n < 0) 
        error("ERROR reading from socket");
}
