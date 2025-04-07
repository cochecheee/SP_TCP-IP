#include "../include/http_server.h"
#include "../include/request_handler.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Khai báo struct client_info ở phạm vi toàn cục
struct client_info {
    int client_fd;
    SSL *ssl;
    int use_ssl;  // Thêm flag để xác định có sử dụng SSL không
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
    int use_ssl = info->use_ssl;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = 0;

    if (use_ssl) {
        // Chấp nhận kết nối SSL
        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
        } else {
            // Nhận dữ liệu từ client qua SSL
            bytes_received = SSL_read(ssl, buffer, sizeof(buffer) - 1);
        }
    } else {
        // Nhận dữ liệu thông thường nếu không sử dụng SSL
        bytes_received = read(client_fd, buffer, sizeof(buffer) - 1);
    }

    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        // Xử lý request với SSL hoặc không
        handle_request(client_fd, buffer, ssl, use_ssl);
    }

    // Đóng kết nối
    if (use_ssl && ssl) {
        SSL_free(ssl);
    }
    close(client_fd);
    free(info);
    return NULL;
}

void start_server(server_config config) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    SSL_CTX *ctx = NULL;

    // Khởi tạo SSL context nếu cần
    if (config.use_ssl) {
        ctx = create_ssl_context();
        load_certificates(ctx, config.cert_file, config.key_file);
        printf("HTTPS Server sẽ lắng nghe trên cổng %d\n", HTTPS_PORT);
    } else {
        printf("HTTP Server sẽ lắng nghe trên cổng %d\n", HTTP_PORT);
    }

    // Tạo socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Không thể tạo socket");
        if (ctx) SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    // Thiết lập để có thể tái sử dụng địa chỉ
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        if (ctx) SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    // Cấu hình socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(config.use_ssl ? HTTPS_PORT : HTTP_PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        if (ctx) SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối
    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        close(server_fd);
        if (ctx) SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    printf("Server đang lắng nghe...\n");

    while (1) {
        // Chấp nhận kết nối từ client
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("accept failed");
            continue;
        }

        // Tạo struct để truyền vào thread
        struct client_info *info = malloc(sizeof(struct client_info));
        if (!info) {
            perror("malloc failed");
            close(client_fd);
            continue;
        }
        
        info->client_fd = client_fd;
        info->use_ssl = config.use_ssl;
        info->ssl = NULL;

        // Khởi tạo SSL cho kết nối nếu cần
        if (config.use_ssl) {
            info->ssl = SSL_new(ctx);
            if (!info->ssl) {
                perror("SSL_new failed");
                free(info);
                close(client_fd);
                continue;
            }
            SSL_set_fd(info->ssl, client_fd);
        }

        // Tạo thread để xử lý client
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, (void *)info);
        pthread_detach(thread_id);
    }

    close(server_fd);
    if (ctx) SSL_CTX_free(ctx);
}

int main(int argc, char *argv[]) {
    server_config config;
    
    // Kiểm tra tham số dòng lệnh
    if (argc >= 3 && strcmp(argv[1], "--ssl") == 0) {
        config.use_ssl = 1;
        config.cert_file = argv[2];
        
        if (argc >= 4) {
            config.key_file = argv[3];
        } else {
            // Sử dụng cùng một file cho cả cert và key nếu không có tham số key
            config.key_file = argv[2];
        }
    } else {
        config.use_ssl = 0;
        config.cert_file = NULL;
        config.key_file = NULL;
    }
    
    start_server(config);
    return 0;
}
