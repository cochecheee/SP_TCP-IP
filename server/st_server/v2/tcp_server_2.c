#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "server_2.h"

int main() {
    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in cli_addr;

    // Khởi tạo server
    sockfd = create_server_socket(8081);
    
    printf("TCP server listening on port 8081\n");

    // Chấp nhận kết nối
    clilen = sizeof(cli_addr);
    newsockfd = accept_connection(sockfd, &cli_addr, &clilen);

    // Vòng lặp giao tiếp với client
    while (1) {
        if (receive_message(newsockfd, buffer, sizeof(buffer)) == 0) {
            printf("Client disconnected\n");
            break;
        }
        printf("Message from client: %s\n", buffer);

        send_message(newsockfd, "Message received");
    }

    // Đóng kết nối
    close(newsockfd);
    close(sockfd);
    return 0;
}
