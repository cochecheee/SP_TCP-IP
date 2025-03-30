#include "server.h"

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int create_socket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    return sockfd;
}

void bind_socket(int sockfd, struct sockaddr_in *serv_addr) {
    bzero((char *) serv_addr, sizeof(*serv_addr));
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_addr.s_addr = INADDR_ANY;
    serv_addr->sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *) serv_addr, sizeof(*serv_addr)) < 0) 
        error("ERROR on binding");
}

int accept_connection(int sockfd, struct sockaddr_in *cli_addr, socklen_t *clilen) {
    int newsockfd = accept(sockfd, (struct sockaddr *) cli_addr, clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");
    return newsockfd;
}

void handle_client(int newsockfd) {
    char buffer[BUFFER_SIZE];
    int n;

    // Nhận dữ liệu từ client
    bzero(buffer, BUFFER_SIZE);
    n = read(newsockfd, buffer, BUFFER_SIZE - 1);
    if (n < 0) error("ERROR reading from socket");

    printf("Message from client: %s\n", buffer);

    // Gửi phản hồi về client
    n = write(newsockfd, "Message received", 16);
    if (n < 0) error("ERROR writing to socket");

    close(newsockfd);
}
