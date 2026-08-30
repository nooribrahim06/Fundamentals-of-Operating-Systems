/*
 * Minimal blocking TCP/HTTP server for learning OS socket primitives.
 *
 * NOT production code.
 * It intentionally handles one accepted connection at a time so that
 * socket(), bind(), listen(), accept(), read(), write(), and close()
 * are easy to see.
 *
 * Safety difference from the course demo:
 * this version binds to 127.0.0.1 instead of 0.0.0.0 so it is local-only.
 */

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BACKLOG 10
#define BUFFER_SIZE 4096

int main(void) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    int yes = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt");
        close(listen_fd);
        return EXIT_FAILURE;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) != 1) {
        fprintf(stderr, "inet_pton failed\n");
        close(listen_fd);
        return EXIT_FAILURE;
    }

    if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind");
        close(listen_fd);
        return EXIT_FAILURE;
    }

    if (listen(listen_fd, BACKLOG) == -1) {
        perror("listen");
        close(listen_fd);
        return EXIT_FAILURE;
    }

    printf("Listening on http://127.0.0.1:%d\n", PORT);

    for (;;) {
        printf("Waiting in accept()...\n");

        struct sockaddr_in client_address;
        socklen_t client_length = sizeof(client_address);

        int client_fd = accept(
            listen_fd,
            (struct sockaddr *)&client_address,
            &client_length
        );

        if (client_fd == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            break;
        }

        char buffer[BUFFER_SIZE];
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

        if (bytes_read == -1) {
            perror("read");
            close(client_fd);
            continue;
        }

        buffer[bytes_read] = '\0';
        printf("\n--- Request bytes ---\n%s\n", buffer);

        const char body[] = "Hello from the C socket server!\n";

        char response[512];
        int response_length = snprintf(
            response,
            sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %zu\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            strlen(body),
            body
        );

        if (response_length < 0 || (size_t)response_length >= sizeof(response)) {
            fprintf(stderr, "response buffer too small\n");
            close(client_fd);
            continue;
        }

        ssize_t total_written = 0;
        while (total_written < response_length) {
            ssize_t n = write(
                client_fd,
                response + total_written,
                (size_t)(response_length - total_written)
            );

            if (n == -1) {
                if (errno == EINTR) {
                    continue;
                }
                perror("write");
                break;
            }

            total_written += n;
        }

        close(client_fd);
    }

    close(listen_fd);
    return EXIT_SUCCESS;
}
