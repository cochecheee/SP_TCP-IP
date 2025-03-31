#include "./include/client_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *handle_client(void *arg) {
    client_info_t *client_info = (client_info_t *)arg;
    int client_socket = client_info->client_socket;
    struct sockaddr_in client_addr = client_info->client_addr;
    char buffer[BUFFER_SIZE];
    int bytes_read;

    // Giải phóng bộ nhớ
    free(arg);

    // Lấy địa chỉ IP và cổng của client
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    int client_port = ntohs(client_addr.sin_port);

    // Nhận và phản hồi dữ liệu
    while ((bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("Received from %s:%d: %s\n", client_ip, client_port, buffer);

        // Gửi lại dữ liệu cho client
        write(client_socket, buffer, bytes_read);
    }

    if (bytes_read == 0) {
        printf("Client %s:%d disconnected\n", client_ip, client_port);
    } else {
        perror("read");
    }

    close(client_socket);
    return NULL;
}
