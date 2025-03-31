#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

void error(const char *msg);
int connect_to_server(const char *server_ip, int port);
void send_message(int sockfd, const char *message);
void receive_message(int sockfd, char *buffer, int size);

#endif // CLIENT_UTILS_H
