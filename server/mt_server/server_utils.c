#include "./include/server.h"
#include "./include/client_handler.h"

int start_server() {
    int server_socket, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    pthread_t tid;

    // Tạo socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }

    // Gán socket với địa chỉ và cổng
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_socket);
        return -1;
    }

    // Lắng nghe kết nối
    if (listen(server_socket, 10) == -1) {
        perror("listen");
        close(server_socket);
        return -1;
    }

    printf("Server listening on port %d...\n", PORT);

    // Chấp nhận kết nối từ client
    while (1) {
        if ((new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len)) == -1) {
            perror("accept");
            continue;
        }

        printf("New connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Cấp phát bộ nhớ cho client
        client_info_t *client_info = malloc(sizeof(client_info_t));
        if (!client_info) {
            perror("malloc");
            close(new_socket);
            continue;
        }

        // Gán thông tin client
        client_info->client_socket = new_socket;
        client_info->client_addr = client_addr;

        // Tạo luồng xử lý client
        if (pthread_create(&tid, NULL, handle_client, client_info) != 0) {
            perror("pthread_create");
            free(client_info);
            close(new_socket);
        }

        pthread_detach(tid);
    }

    close(server_socket);
    return 0;
}
