#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

void send_response(int client_fd, const char *status, const char *content_type, const char *body);

#endif