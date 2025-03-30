#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    char buffer[256];

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(1);
    }

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    // Initialize server address structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[2]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        error("ERROR invalid address or address not supported");
    }

    // Connect to server
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    // Continuous communication loop
    while (1) {
        // Get message from user
        printf("Please enter the message (or type 'exit' to quit): ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);

        // Remove newline character from the input
        buffer[strcspn(buffer, "\n")] = 0;

        // Check if the user wants to exit
        if (strcmp(buffer, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }

        // Send message to server
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) 
            error("ERROR writing to socket");

        // Read response from server
        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);
        if (n < 0) 
            error("ERROR reading from socket");
        printf("Server response: %s\n", buffer);
    }

    // Close socket
    close(sockfd);
    return 0;
}
