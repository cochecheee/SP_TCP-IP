#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "client.h"

int main(int argc, char *argv[]) {
    int sockfd, n;
    char buffer[256];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        exit(1);
    }

    // Kết nối đến server
    sockfd = connect_to_server(argv[1], atoi(argv[2]));

    // Vòng lặp giao tiếp với server
    while (1) {
        printf("Enter message (or type 'exit' to quit): ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        buffer[strcspn(buffer, "\n")] = 0;  // Xóa ký tự xuống dòng

        if (strcmp(buffer, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }

        // Gửi tin nhắn đến server
        send_message(sockfd, buffer);

        // Nhận phản hồi từ server
        receive_message(sockfd, buffer, sizeof(buffer));
        printf("Server response: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
