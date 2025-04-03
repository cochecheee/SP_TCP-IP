#include "../include/http_server.h"
#include "../include/http_request.h"
#include "../include/http_response.h"
#include "../include/file_utils.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void *handle_client(void *arg) {
    int client_fd = *((int *)arg);
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));

    // Nhận request data từ client và lưu vào buffer
    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0) {
        // Parse request để lấy tên file
        char *file_name = parse_http_request(buffer, bytes_received);
        
        if (file_name != NULL) {
            // Lấy phần mở rộng file
            const char *file_ext = get_file_extension(file_name);
            
            // Tạo HTTP response
            char *response = (char *)malloc(BUFFER_SIZE * 2 * sizeof(char));
            size_t response_len;
            build_http_response(file_name, file_ext, response, &response_len);

            // Gửi HTTP response đến client
            send(client_fd, response, response_len, 0);

            free(response);
            free(file_name);
        }
    }
    
    close(client_fd);
    free(arg);
    free(buffer);
    return NULL;
}

int start_http_server(int port) {
    int server_fd;
    struct sockaddr_in server_addr;

    // Tạo server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        return -1;
    }
    
    // Cấu hình socket để có thể tái sử dụng ngay địa chỉ và port
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        return -1;
    }

    // Cấu hình địa chỉ server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind socket vào port
    if (bind(server_fd, 
            (struct sockaddr *)&server_addr, 
            sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        return -1;
    }

    // Lắng nghe kết nối
    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        close(server_fd);
        return -1;
    }

    printf("Server listening on port %d\n", port);
    
    // Main server loop
    while (1) {
        // Thông tin client
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int *client_fd = malloc(sizeof(int));

        // Chấp nhận kết nối client
        if ((*client_fd = accept(server_fd, 
                               (struct sockaddr *)&client_addr, 
                               &client_addr_len)) < 0) {
            perror("accept failed");
            free(client_fd);
            continue;
        }

        // Hiển thị thông tin client kết nối
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Client connected: %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        // Tạo thread mới để xử lý request client
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, (void *)client_fd) != 0) {
            perror("pthread_create failed");
            close(*client_fd);
            free(client_fd);
            continue;
        }
        pthread_detach(thread_id);
    }

    close(server_fd);
    return 0;
}