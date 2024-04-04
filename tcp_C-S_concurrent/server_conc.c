//Name: Pankaj Sah
//Roll No: 21CS8122


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 58122
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

// Function to check if the given string has balanced parentheses, braces, and brackets
int is_well_balanced(char *s) {
    char stack[BUFFER_SIZE];
    int top = -1;
    char mapping[3][2] = {{')', '('}, {'}', '{'}, {']', '['}};

    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] == '(' || s[i] == '{' || s[i] == '[') {
            // Opening bracket, push onto the stack
            stack[++top] = s[i];
        } else if (s[i] == ')' || s[i] == '}' || s[i] == ']') {
            // Closing bracket, check if it matches the top of the stack
            if (top == -1 || stack[top--] != mapping[s[i] - 41][1]) {
                return 0; // Unbalanced
            }
        }
    }
    return (top == -1); // If the stack is empty, the brackets are well-balanced
}

// Function to handle communication with a client
void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received;
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytes_received] = '\0';
        int balanced = is_well_balanced(buffer);
        char* response = (balanced) ? "Yes" : "No";
        send(client_socket, response, strlen(response), 0);
    }
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create a socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the specified address and port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("[*] Server listening on localhost:%d\n", PORT);

    while (1) {
        // Accept incoming connection
        if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
            perror("Acceptance failed");
            exit(EXIT_FAILURE);
        }

        printf("[*] Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Fork to handle the client in a separate process
        pid_t pid = fork();
        if (pid == 0) {
            close(server_socket); // Close the listening socket in the child process
            handle_client(client_socket);
            exit(EXIT_SUCCESS);
        } else if (pid < 0) {
            perror("Forking failed");
            exit(EXIT_FAILURE);
        } else {
            close(client_socket); // Close the client socket in the parent process
        }
    }

    close(server_socket); // This part will not be reached in the current infinite loop

    return 0;
}
