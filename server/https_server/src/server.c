#include "../include/server.h"
#include "../include/ssl.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>

Server* server_create() {
    Server *server = malloc(sizeof(Server));
    if (!server) return NULL;
    
    server->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->fd < 0) {
        free(server);
        return NULL;
    }
    
    // Set SO_REUSEADDR to avoid "address already in use" errors
    int opt = 1;
    setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    return server;
}

bool server_bind(Server *server, int port) {
    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY;
    server->address.sin_port = htons(port);
    
    if (bind(server->fd, (struct sockaddr *)&server->address, sizeof(server->address)) < 0) {
        return false;
    }
    return true;
}

void server_listen(Server *server) {
    listen(server->fd, MAX_CLIENTS);
    printf("Server listening on port %d\n", PORT);
}

void server_destroy(Server *server) {
    if (server) {
        close(server->fd);
        free(server);
    }
}

void* client_handler(void *arg) {
    int client_fd = *((int *)arg);
    free(arg);
    
    handle_client(client_fd);
    close(client_fd);
    return NULL;
}

void accept_connections(Server *server, bool use_ssl) {
    SSL_CTX *ctx = NULL;
    if (use_ssl) {
        ssl_init();
        ctx = create_ssl_context();
        configure_ssl_context(ctx, "../cert.pem", "../key.pem");
    }
    
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int *client_fd = malloc(sizeof(int));
        
        *client_fd = accept(server->fd, (struct sockaddr *)&client_addr, &client_len);
        if (*client_fd < 0) {
            perror("accept failed");
            free(client_fd);
            continue;
        }
        
        if (use_ssl) {
            SSL *ssl = SSL_new(ctx);
            SSL_set_fd(ssl, *client_fd);
            
            if (SSL_accept(ssl) <= 0) {
                ERR_print_errors_fp(stderr);
                SSL_free(ssl);
                close(*client_fd);
                free(client_fd);
                continue;
            }
            
            // Create a new thread for SSL client
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, (void *(*)(void *))ssl_handle_client, ssl);
            pthread_detach(thread_id);
        } else {
            // Create a new thread for regular client
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, client_handler, client_fd);
            pthread_detach(thread_id);
        }
    }
    
    if (use_ssl && ctx) {
        SSL_CTX_free(ctx);
        ssl_cleanup();
    }
}