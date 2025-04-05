#include "../include/utils.h"
#include <stdio.h>  // Thêm cho sscanf
#include <stdlib.h> // Thêm cho malloc, free
#include <string.h> // Thêm cho strlen
#include <ctype.h>  // Thêm cho tolower

const char *get_mime_type(const char *file_ext) {
    if (strcasecmp(file_ext, "html") == 0 || strcasecmp(file_ext, "htm") == 0) {
        return "text/html";
    } else if (strcasecmp(file_ext, "txt") == 0) {
        return "text/plain";
    } else if (strcasecmp(file_ext, "jpg") == 0 || strcasecmp(file_ext, "jpeg") == 0) {
        return "image/jpeg";
    } else if (strcasecmp(file_ext, "png") == 0) {
        return "image/png";
    } else if (strcasecmp(file_ext, "ico") == 0) {
        return "image/x-icon"; // Thêm MIME type cho file .ico
    } else {
        return "application/octet-stream";
    }
}

char *url_decode(const char *src) {
    size_t src_len = strlen(src);
    char *decoded = malloc(src_len + 1);
    if (decoded == NULL) {
        return NULL; // Trả về NULL nếu không thể cấp phát bộ nhớ
    }

    size_t decoded_len = 0;

    // Decode %2x to hex
    for (size_t i = 0; i < src_len; i++) {
        if (src[i] == '%' && i + 2 < src_len) {
            int hex_val;
            sscanf(src + i + 1, "%2x", &hex_val);
            decoded[decoded_len++] = hex_val;
            i += 2;
        } else {
            decoded[decoded_len++] = src[i];
        }
    }

    // Add null terminator
    decoded[decoded_len] = '\0';
    return decoded;
}