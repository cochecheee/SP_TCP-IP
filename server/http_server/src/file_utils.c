#include "../include/file_utils.h"
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

const char *get_file_extension(const char *file_name) {
    if (file_name == NULL) {
        return "";
    }

    const char *dot = strrchr(file_name, '.');
    if (!dot || dot == file_name) {
        return "";
    }
    return dot + 1;
}

bool case_insensitive_compare(const char *s1, const char *s2) {
    if (s1 == NULL || s2 == NULL) {
        return s1 == s2;
    }

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
    if (file_name == NULL) {
        return NULL;
    }

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