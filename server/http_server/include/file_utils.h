#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdbool.h>

/**
 * Lấy phần mở rộng từ tên file
 * 
 * @param file_name Tên file
 * @return Con trỏ đến phần mở rộng file (không cần giải phóng)
 */
const char *get_file_extension(const char *file_name);

/**
 * Tìm file bằng cách không phân biệt hoa thường
 * 
 * @param file_name Tên file cần tìm
 * @return Tên file thực tế trong hệ thống (không cần giải phóng)
 */
char *get_file_case_insensitive(const char *file_name);

/**
 * So sánh hai chuỗi không phân biệt hoa thường
 * 
 * @param s1 Chuỗi thứ nhất
 * @param s2 Chuỗi thứ hai
 * @return true nếu hai chuỗi giống nhau, false nếu khác
 */
bool case_insensitive_compare(const char *s1, const char *s2);

#endif // FILE_UTILS_H