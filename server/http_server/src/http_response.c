#include "../include/http_response.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// Trong http_response.c, hàm get_mime_type đã được cập nhật để hỗ trợ CSS và JS
const char *get_mime_type(const char *file_ext) {
    if (!file_ext) {
        return "application/octet-stream";
    }
    if (strcasecmp(file_ext, "html") == 0 || strcasecmp(file_ext, "htm") == 0) {
        return "text/html";
    } else if (strcasecmp(file_ext, "txt") == 0) {
        return "text/plain";
    } else if (strcasecmp(file_ext, "css") == 0) {
        return "text/css";
    } else if (strcasecmp(file_ext, "js") == 0) {
        return "application/javascript";
    } else if (strcasecmp(file_ext, "jpg") == 0 || strcasecmp(file_ext, "jpeg") == 0) {
        return "image/jpeg";
    } else if (strcasecmp(file_ext, "png") == 0) {
        return "image/png";
    } else if (strcasecmp(file_ext, "gif") == 0) {
        return "image/gif";
    } else if (strcasecmp(file_ext, "svg") == 0) {
        return "image/svg+xml";
    } else if (strcasecmp(file_ext, "ico") == 0) {
        return "image/x-icon";
    } else if (strcasecmp(file_ext, "json") == 0) {
        return "application/json";
    } else if (strcasecmp(file_ext, "pdf") == 0) {
        return "application/pdf";
    } else if (strcasecmp(file_ext, "woff") == 0) {
        return "font/woff";
    } else if (strcasecmp(file_ext, "woff2") == 0) {
        return "font/woff2";
    } else if (strcasecmp(file_ext, "ttf") == 0) {
        return "font/ttf";
    } else if (strcasecmp(file_ext, "eot") == 0) {
        return "application/vnd.ms-fontobject";
    } else if (strcasecmp(file_ext, "otf") == 0) {
        return "font/otf";
    } else if (strcasecmp(file_ext, "xml") == 0) {
        return "application/xml";
    } else {
        return "application/octet-stream";
    }
}

void build_http_response(const char *file_name, 
                        const char *file_ext, 
                        char *response, 
                        size_t *response_len) {
    // Mở file để đọc
    int file_fd = open(file_name, O_RDONLY);
    
    // Nếu file không tồn tại, trả về 404 Not Found
    if (file_fd == -1) {
        snprintf(response, BUFFER_SIZE,
                 "HTTP/1.1 404 Not Found\r\n"
                 "Content-Type: text/plain\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "404 Not Found: The requested file '%s' was not found on this server.",
                 file_name);
        *response_len = strlen(response);
        return;
    }

    // Lấy kích thước file để đặt Content-Length
    struct stat file_stat;
    fstat(file_fd, &file_stat);
    off_t file_size = file_stat.st_size;

    // Tạo HTTP header
    const char *mime_type = get_mime_type(file_ext);
    *response_len = snprintf(response, BUFFER_SIZE,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: %s\r\n"
                "Content-Length: %ld\r\n"
                "Connection: close\r\n"
                "\r\n",
                mime_type, (long)file_size);

    // Sao chép nội dung file vào response buffer
    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, 
                            response + *response_len, 
                            BUFFER_SIZE - *response_len)) > 0) {
        *response_len += bytes_read;
    }

    close(file_fd);
}