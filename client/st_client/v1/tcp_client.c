#include "client.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(1);
    }

    int portno = atoi(argv[2]);
    int sockfd = create_socket();
    struct sockaddr_in serv_addr;
    struct hostent *server = resolve_host(argv[1]);

    connect_to_server(sockfd, &serv_addr, server, portno);
    communicate_with_server(sockfd);

    close(sockfd);
    return 0;
}
