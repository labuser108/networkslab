//Name: Pankaj Sah
//RollNo:-21CS8122

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_EXPRESSION_LENGTH 100


int isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

// Function to perform the operation
int performOperation(int operand1, int operand2, char operation) {
    switch (operation) {
        case '+':
            return operand1 + operand2;
        case '-':
            return operand1 - operand2;
        case '*':
            return operand1 * operand2;
        case '/':
            return operand1 / operand2;
        default:
            return 0;  // Invalid
    }
}

// Function to evaluate and print the result of the arithmetic expression
int calculateExpression(char *expression) {
    int result = 0;
    int operand1, operand2;
    char operation = '+';

    while (*expression) {
        // Skip spaces
        if (*expression == ' ') {
            expression++;
            continue;
        }

        if (isOperator(*expression)) {
            // If the current character is an operator, update the operation
            operation = *expression;
        } else {
            // If it's a digit, convert the substring to an integer
            operand2 = strtol(expression, (char **)&expression, 10);

            // Perform the operation with the previous result and the new operand
            result = performOperation(result, operand2, operation);
            continue;
        }

        // Move to the next character
        expression++;
    }
    return result;
}


int main() {
    // Set up the server socket
    int serverSocket, clientSocket, port;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;

    // Change the port number based on your roll number
    port = 50122;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding socket");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    listen(serverSocket, 5);

    printf("Server listening on port %d...\n", port);

    // Accept connections and handle expressions
    while (1) {
        addr_size = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addr_size);
        if (clientSocket == -1) {
            perror("Error accepting connection");
            continue;
        }

        char expression[MAX_EXPRESSION_LENGTH];
        ssize_t bytesReceived = recv(clientSocket, expression, sizeof(expression), 0);
        if (bytesReceived == -1) {
            perror("Error receiving data");
            close(clientSocket);
            continue;
        }

        expression[bytesReceived] = '\0'; // Null-terminate the received data

        int result = calculateExpression(expression);

        printf("Received expression: %s\n", expression);
        printf("Result: %d\n", result);

        // Send the result back to the client
        send(clientSocket, &result, sizeof(result), 0);

        close(clientSocket);
    }

    close(serverSocket);

    return 0;
}

