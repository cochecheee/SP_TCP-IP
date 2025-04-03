#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <stddef.h>
#define BUFFER_SIZE 1024

/**
 * Tạo HTTP response từ file
 * 
 * @param file_name Tên file để đưa vào response
 * @param file_ext Phần mở rộng file
 * @param response Buffer để lưu response
 * @param response_len Con trỏ đến biến lưu độ dài response
 */
void build_http_response(const char *file_name, 
                         const char *file_ext,
                         char *response, 
                         size_t *response_len);

/**
 * Lấy MIME type dựa vào phần mở rộng file
 * 
 * @param file_ext Phần mở rộng file
 * @return MIME type tương ứng
 */
const char *get_mime_type(const char *file_ext);

#endif // HTTP_RESPONSE_H