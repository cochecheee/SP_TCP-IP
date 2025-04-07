#include "../include/http_server.h"
#include "../include/request_handler.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct client_info {
    int client_fd;
};

void *handle_client(void *arg) {
    struct client_info *info = (struct client_info *)arg;
    int client_fd = info->client_fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = 0;

    bytes_received = read(client_fd, buffer, sizeof(buffer) - 1);

    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        handle_request(client_fd, buffer);
    }

    close(client_fd);
    free(info);
    return NULL;
}

void start_server() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Không thể tạo socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(HTTP_PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("HTTP Server đang lắng nghe trên cổng %d...\n", HTTP_PORT);

    while (1) {
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("accept failed");
            continue;
        }

        struct client_info *info = malloc(sizeof(struct client_info));
        if (!info) {
            perror("malloc failed");
            close(client_fd);
            continue;
        }
        
        info->client_fd = client_fd;

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, (void *)info);
        pthread_detach(thread_id);
    }

    close(server_fd);
}

int main(int argc, char *argv[]) {
    start_server();
    return 0;
}