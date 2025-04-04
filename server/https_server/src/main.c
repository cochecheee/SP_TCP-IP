#include "../include/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

void print_usage(const char *program_name) {
    printf("Usage: %s [options]\n", program_name);
    printf("Options:\n");
    printf("  -p, --port <port>      Specify port to listen on (default: 8080)\n");
    printf("  -s, --ssl              Enable SSL/TLS\n");
    printf("  -h, --help             Show this help message\n");
}

int main(int argc, char *argv[]) {
    int port = PORT;
    bool use_ssl = false;
    
    // Parse command line arguments
    int opt;
    while ((opt = getopt(argc, argv, "p:sh")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                if (port <= 0 || port > 65535) {
                    fprintf(stderr, "Invalid port number\n");
                    return EXIT_FAILURE;
                }
                break;
            case 's':
                use_ssl = true;
                break;
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }
    
    Server *server = server_create();
    if (!server) {
        perror("Failed to create server");
        return EXIT_FAILURE;
    }
    
    if (!server_bind(server, port)) {
        perror("Failed to bind server");
        server_destroy(server);
        return EXIT_FAILURE;
    }
    
    server_listen(server);
    accept_connections(server, use_ssl);
    
    server_destroy(server);
    return EXIT_SUCCESS;
}