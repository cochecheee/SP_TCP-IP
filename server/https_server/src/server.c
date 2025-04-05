#include "../include/http_server.h"
#include "../include/request_handler.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Khai báo struct client_info ở phạm vi toàn cục
struct client_info {
    int client_fd;
    SSL *ssl;
};

SSL_CTX *create_ssl_context();
void load_certificates(SSL_CTX *ctx, const char *cert_file, const char *key_file);

// Khởi tạo SSL context
SSL_CTX *create_ssl_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    // Khởi tạo OpenSSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    // Sử dụng SSLv23_server_method thay cho TLS_server_method để tương thích rộng hơn
    method = SSLv23_server_method();
    if (!method) {
        perror("Unable to create SSL method");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

// Tải chứng chỉ và khóa
void load_certificates(SSL_CTX *ctx, const char *cert_file, const char *key_file) {
    if (SSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

void *handle_client(void *arg) {
    struct client_info *info = (struct client_info *)arg;
    int client_fd = info->client_fd;
    SSL *ssl = info->ssl;
    char buffer[BUFFER_SIZE];

    // Chấp nhận kết nối SSL
    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
    } else {
        // Nhận dữ liệu từ client qua SSL
        ssize_t bytes_received = SSL_read(ssl, buffer, sizeof(buffer) - 1);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            // Truyền thêm tham số ssl vào handle_request
            handle_request(client_fd, buffer, ssl);
        }
    }

    // Đóng kết nối SSL và socket
    SSL_free(ssl);
    close(client_fd);
    free(info);
    return NULL;
}

void start_server() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Khởi tạo SSL context
    SSL_CTX *ctx = create_ssl_context();
    load_certificates(ctx, "server.crt", "server.key");

    // Tạo socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    // Cấu hình socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối
    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        close(server_fd);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    printf("HTTPS Server listening on port %d\n", PORT);

    while (1) {
        // Chấp nhận kết nối từ client
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("accept failed");
            continue;
        }

        // Tạo SSL object cho kết nối
        SSL *ssl = SSL_new(ctx);
        if (!ssl) {
            perror("SSL_new failed");
            close(client_fd);
            continue;
        }
        SSL_set_fd(ssl, client_fd);

        // Tạo struct để truyền vào thread
        struct client_info *info = malloc(sizeof(struct client_info));
        if (!info) {
            perror("malloc failed");
            SSL_free(ssl);
            close(client_fd);
            continue;
        }
        info->client_fd = client_fd;
        info->ssl = ssl;

        // Tạo thread để xử lý client
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, (void *)info);
        pthread_detach(thread_id);
    }

    close(server_fd);
    SSL_CTX_free(ctx);
}

int main() {
    start_server();
    return 0;
}