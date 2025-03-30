#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

void error(const char *msg);
int create_socket();
void connect_to_server(int sockfd, const char *hostname, int portno);
void send_message(int sockfd, const char *message);
void receive_message(int sockfd, char *buffer, int size);

#endif 
