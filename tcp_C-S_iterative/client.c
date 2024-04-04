//Name: Pankaj Sah
//RollNo:-21CS8122


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_EXPRESSION_LENGTH 100

int main() {
    int clientSocket, port;
    struct sockaddr_in serverAddr;

    port = 50122;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to server");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server on port %d...\n", port);

    // Client loop
    while (1) {
        char expression[MAX_EXPRESSION_LENGTH];
        printf("Enter an arithmetic expression (or -1 to exit): ");
        fgets(expression, sizeof(expression), stdin);

        // Remove trailing newline character
        expression[strcspn(expression, "\n")] = '\0';

        // Exit loop if the user enters -1
        if (strcmp(expression, "-1") == 0) {
            break;
        }

        // Send the expression to the server
        ssize_t bytesSent = send(clientSocket, expression, strlen(expression), 0);
        if (bytesSent == -1) {
            perror("Error sending data");
            break;
        }

        int result;
        // Receive the result from the server
        ssize_t bytesReceived = recv(clientSocket, &result, sizeof(result), 0);
        if (bytesReceived == -1) {
            perror("Error receiving data");
            break;
        }

        // Display the result
        printf("Result: %d\n", result);
    }

    close(clientSocket);

    return 0;
}

