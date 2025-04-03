#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <stdbool.h>
#include <stddef.h>

/**
 * Phân tích request HTTP và trích xuất tên file
 * 
 * @param request Buffer chứa request HTTP
 * @param request_size Kích thước request
 * @return Con trỏ tới tên file đã được decode (cần được giải phóng khi sử dụng xong)
 */
char *parse_http_request(const char *request, size_t request_size);

/**
 * Decode URL encoded string
 * 
 * @param src Chuỗi URL encoded 
 * @return Chuỗi đã được decode (cần được giải phóng khi sử dụng xong)
 */
char *url_decode(const char *src);

#endif // HTTP_REQUEST_H