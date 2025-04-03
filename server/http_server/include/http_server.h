#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <stdbool.h>
#include <stddef.h>

#define PORT 8080
#define BUFFER_SIZE 104857600

/**
 * Khởi tạo và chạy HTTP server
 * 
 * @param port Port để lắng nghe kết nối
 * @return 0 nếu thành công, -1 nếu thất bại
 */
int start_http_server(int port);

/**
 * Xử lý kết nối client mới
 * 
 * @param arg Con trỏ tới client socket file descriptor
 * @return NULL
 */
void *handle_client(void *arg);

#endif // HTTP_SERVER_H