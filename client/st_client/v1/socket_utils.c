#include "client.h"

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

struct hostent *resolve_host(const char *hostname) {
    struct hostent *server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }
    return server;
}

void connect_to_server(int sockfd, struct sockaddr_in *serv_addr, struct hostent *server, int portno) {
    bzero((char *) serv_addr, sizeof(*serv_addr));
    serv_addr->sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr->sin_addr.s_addr, server->h_length);
    serv_addr->sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) serv_addr, sizeof(*serv_addr)) < 0) 
        error("ERROR connecting");
}

void communicate_with_server(int sockfd) {
    char buffer[BUFFER_SIZE];
    int n;

    while (1) {
        // Nhập tin nhắn từ người dùng
        printf("Please enter the message: ");
        fflush(stdout);
        bzero(buffer, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE - 1, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Xóa ký tự xuống dòng

        if (strcmp(buffer, "exit") == 0) {
            puts("Exiting...");
            break;
        }

        // Gửi dữ liệu đến server
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) 
            error("ERROR writing to socket");

        // Đọc phản hồi từ server
        bzero(buffer, BUFFER_SIZE);
        n = read(sockfd, buffer, BUFFER_SIZE - 1);
        if (n < 0) 
            error("ERROR reading from socket");

        printf("Server response: %s\n", buffer);
    }
}
