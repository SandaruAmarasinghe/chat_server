#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef _WIN32
#include <winsock2.h>
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#define PORT 6001
#define MAX_CLIENTS 10

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int server_socket, client_socket[MAX_CLIENTS], client_count = 0;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return EXIT_FAILURE;
    }
#endif

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
        error("Error opening socket");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        error("Error on binding");

    if (listen(server_socket, MAX_CLIENTS) < 0)
        error("Error on listen");

    printf("Server started. Waiting for connections...\n");

    while (1) {
        client_len = sizeof(client_addr);
        client_socket[client_count] = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket[client_count] < 0)
            error("Error on accept");

        printf("New client connected.\n");

        if (fork() == 0) {
            close(server_socket);
            while (1) {
                char buffer[256];
                ssize_t n = read(client_socket[client_count], buffer, sizeof(buffer) - 1);
                if (n < 0) {
                    perror("Error reading from socket");
                    exit(EXIT_FAILURE);
                } else if (n == 0) {
                    printf("Connection closed by client.\n");
                    break;
                }
                buffer[n] = '\0'; 
                printf("Client: %s\n", buffer);
                write(client_socket[client_count], "Message received", strlen("Message received"));
            }
            close(client_socket[client_count]);
            exit(EXIT_SUCCESS);
        } else {
            close(client_socket[client_count]); 
            client_count++;
        }
    }

    close(server_socket);
    return EXIT_SUCCESS;
}
