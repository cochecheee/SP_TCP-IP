#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <pthread.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define HTTP_PORT 8080        // Thêm HTTP port
#define HTTPS_PORT 9443       
#define BUFFER_SIZE 4096
#define DOCUMENT_ROOT "./www"

typedef struct {
    int use_ssl;             // Flag để xác định có sử dụng SSL không
    char *cert_file;         // Đường dẫn đến file chứng chỉ
    char *key_file;          // Đường dẫn đến file khóa
} server_config;

void *handle_client(void *arg);
void start_server(server_config config);

#endif
