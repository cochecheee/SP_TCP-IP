#include "../include/http_request.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *parse_http_request(const char *request, size_t request_size) {
    // Khởi tạo regex để tìm tên file từ request
    regex_t regex;
    regcomp(&regex, "^GET /([^ ]*) HTTP/1", REG_EXTENDED);
    regmatch_t matches[2];
    char *file_name = NULL;

    // Thực hiện tìm kiếm bằng regex
    if (regexec(&regex, request, 2, matches, 0) == 0) {
        // Sao chép phần URL từ request
        size_t url_len = matches[1].rm_eo - matches[1].rm_so;
        char *url_encoded_file_name = malloc(url_len + 1);
        
        if (url_encoded_file_name != NULL) {
            strncpy(url_encoded_file_name, request + matches[1].rm_so, url_len);
            url_encoded_file_name[url_len] = '\0';
            
            // Decode URL
            file_name = url_decode(url_encoded_file_name);
            free(url_encoded_file_name);
        }
    }

    regfree(&regex);
    return file_name;
}

char *url_decode(const char *src) {
    if (src == NULL) {
        return NULL;
    }
    
    size_t src_len = strlen(src);
    char *decoded = malloc(src_len + 1);
    
    if (decoded == NULL) {
        return NULL;
    }
    
    size_t decoded_len = 0;

    // Decode %2x thành hex
    for (size_t i = 0; i < src_len; i++) {
        if (src[i] == '%' && i + 2 < src_len) {
            int hex_val;
            sscanf(src + i + 1, "%2x", &hex_val);
            decoded[decoded_len++] = hex_val;
            i += 2;
        } else if (src[i] == '+') {
            // Convert '+' thành space
            decoded[decoded_len++] = ' ';
        } else {
            decoded[decoded_len++] = src[i];
        }
    }

    // Thêm null terminator
    decoded[decoded_len] = '\0';
    
    // Xử lý tên file rỗng hoặc "/"
    if (strlen(decoded) == 0 || strcmp(decoded, "/") == 0) {
        free(decoded);
        return strdup("www/index.html");
    }
    
    return decoded;
}