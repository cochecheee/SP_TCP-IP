#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdbool.h>
#include <stddef.h>  // for size_t
#include <sys/types.h>

#define BUFFER_SIZE 104857600

void build_http_response(const char *file_name, 
                        const char *file_ext, 
                        char *response, 
                        size_t *response_len);
void send_404_response(char *response, size_t *response_len);
void send_500_response(char *response, size_t *response_len);
void handle_client(int client_fd);

#endif /* RESPONSE_H */