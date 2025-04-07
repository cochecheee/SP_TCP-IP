#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

void handle_request(int client_fd, const char *request);
void handle_php_request(int client_fd, const char *file_path, const char *method, const char *body, const char *headers);

#endif