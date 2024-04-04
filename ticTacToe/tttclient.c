#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int client_socket;
char board[9];

void display_board() {
    printf("%c | %c | %c\n", board[0], board[1], board[2]);
    printf("--+---+--\n");
    printf("%c | %c | %c\n", board[3], board[4], board[5]);
    printf("--+---+--\n");
    printf("%c | %c | %c\n", board[6], board[7], board[8]);
}

int get_move() {
    int move;
    while (1) {
        printf("Enter your move (0-8) : ");
        scanf("%d", &move);
        if (move < 0 || move > 8) {
            printf("Invalid move! Please enter a number between 0 and 8.\n");
        } else {
            return move;
        }
    }
}

void receive_message() {
    char buffer[BUFFER_SIZE];
    while (1) {
        recv(client_socket, buffer, BUFFER_SIZE, 0);
        printf("%s", buffer);
        if (strstr(buffer, "Your turn") != NULL) {
            break;
        } else if (strstr(buffer, "win") != NULL || strstr(buffer, "lose") != NULL) {
            close(client_socket);
            exit(EXIT_SUCCESS);
        }
    }
}

int main() {
    struct sockaddr_in server_addr;
    
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        display_board();
        receive_message();
        int move = get_move();
        send(client_socket, &move, sizeof(move), 0);
    }

    return 0;
}

