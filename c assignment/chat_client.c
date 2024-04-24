#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h> 

#define PORT 6001
#define SERVER_IP "192.168.8.173" // Replace with the IP address of your VMware virtual machine

void error(const char *msg)
{
      perror(msg);
      exit(1);
}

int main()
{
      int client_socket;
      struct sockaddr_in server_addr;
      char buffer[256];

      WSADATA wsa_data;
      if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
            fprintf(stderr, "WSAStartup failed\n");
            return 1;
      }

      client_socket = socket(AF_INET, SOCK_STREAM, 0);
      if (client_socket < 0)
            error("Error opening socket");

      memset(&server_addr, 0, sizeof(server_addr));
      server_addr.sin_family = AF_INET;
      server_addr.sin_port = htons(PORT);

      if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
            error("Invalid address");

      if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
            error("Connection failed");

      printf("Connected to server.\n");

      while (1)
      {
            printf("Enter message: ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0; 
            send(client_socket, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            recv(client_socket, buffer, sizeof(buffer), 0);
            printf("Server: %s\n", buffer);
      }

      closesocket(client_socket);
      WSACleanup();
      return 0;
}
