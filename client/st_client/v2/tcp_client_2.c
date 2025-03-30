#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "client_2.h"

int main(int argc, char *argv[]) {
    int sockfd, portno;
    char buffer[256];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
        exit(1);
    }

    portno = atoi(argv[2]);
    
    // Tạo và kết nối socket
    sockfd = create_socket();
    connect_to_server(sockfd, argv[1], portno);

    // Vòng lặp giao tiếp
    while (1) {
        printf("Please enter the message (or type 'exit' to quit): ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }

        send_message(sockfd, buffer);
        receive_message(sockfd, buffer, sizeof(buffer));
        printf("Server response: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
