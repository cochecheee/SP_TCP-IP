#include "../include/http_server.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int port = PORT; // Sử dụng port mặc định

    // Nếu cung cấp port qua tham số dòng lệnh
    if (argc > 1) {
        port = atoi(argv[1]);
    }

    printf("Starting HTTP server on port %d...\n", port);
    if (start_http_server(port) != 0) {
        fprintf(stderr, "Failed to start server\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}