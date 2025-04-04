#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

const char *get_file_extension(const char *file_name);
const char *get_mime_type(const char *file_ext);
bool case_insensitive_compare(const char *s1, const char *s2);
char *get_file_case_insensitive(const char *file_name);
char *url_decode(const char *src);

#endif