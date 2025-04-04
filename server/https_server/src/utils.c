#include "../include/utils.h"
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>

const char *get_file_extension(const char *file_name) {
    const char *dot = strrchr(file_name, '.');
    if (!dot || dot == file_name) {
        return "";
    }
    return dot + 1;
}

const char *get_mime_type(const char *file_ext) {
    if (strcasecmp(file_ext, "html") == 0 || strcasecmp(file_ext, "htm") == 0) {
        return "text/html";
    } else if (strcasecmp(file_ext, "txt") == 0) {
        return "text/plain";
    } else if (strcasecmp(file_ext, "jpg") == 0 || strcasecmp(file_ext, "jpeg") == 0) {
        return "image/jpeg";
    } else if (strcasecmp(file_ext, "png") == 0) {
        return "image/png";
    } else if (strcasecmp(file_ext, "css") == 0) {
        return "text/css";
    } else if (strcasecmp(file_ext, "js") == 0) {
        return "application/javascript";
    } else if (strcasecmp(file_ext, "json") == 0) {
        return "application/json";
    } else {
        return "application/octet-stream";
    }
}

bool case_insensitive_compare(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2)) {
            return false;
        }
        s1++;
        s2++;
    }
    return *s1 == *s2;
}

char *get_file_case_insensitive(const char *file_name) {
    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("opendir");
        return NULL;
    }

    struct dirent *entry;
    char *found_file_name = NULL;
    while ((entry = readdir(dir)) != NULL) {
        if (case_insensitive_compare(entry->d_name, file_name)) {
            found_file_name = strdup(entry->d_name);
            break;
        }
    }

    closedir(dir);
    return found_file_name;
}

char *url_decode(const char *src) {
    size_t src_len = strlen(src);
    char *decoded = malloc(src_len + 1);
    if (!decoded) return NULL;
    
    size_t decoded_len = 0;

    for (size_t i = 0; i < src_len; i++) {
        if (src[i] == '%' && i + 2 < src_len) {
            int hex_val;
            if (sscanf(src + i + 1, "%2x", &hex_val) == 1) {
                decoded[decoded_len++] = hex_val;
                i += 2;
            } else {
                decoded[decoded_len++] = src[i];
            }
        } else if (src[i] == '+') {
            decoded[decoded_len++] = ' ';
        } else {
            decoded[decoded_len++] = src[i];
        }
    }

    decoded[decoded_len] = '\0';
    return decoded;
}