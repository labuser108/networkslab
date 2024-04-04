//Name: Pankaj Sah
//Roll No: 21CS8122

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 58122
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    char input_str[BUFFER_SIZE];

    while (1) {
        printf("Enter a string of parentheses (max 100 characters, enter -1 to exit): ");
        fgets(input_str, BUFFER_SIZE, stdin);
        input_str[strcspn(input_str, "\n")] = '\0';

        if (strcmp(input_str, "-1") == 0) {
            break;
        }

        send(client_socket, input_str, strlen(input_str), 0);

        char response[BUFFER_SIZE];
        recv(client_socket, response, BUFFER_SIZE, 0);
        printf("Result: %s\n", response);
    }

    close(client_socket);

    return 0;
}
