#include "server.h"

int main() {
    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    // Tạo socket
    sockfd = create_socket();

    // Gán socket với cổng
    bind_socket(sockfd, &serv_addr);

    // Lắng nghe kết nối
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    printf("TCP server listening on port %d\n", PORT);

    // Chấp nhận và xử lý kết nối từ client
    newsockfd = accept_connection(sockfd, &cli_addr, &clilen);
    handle_client(newsockfd);

    // Đóng socket
    close(sockfd);
    return 0;
}
