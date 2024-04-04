//Name-Pankaj Sah
//RollNo-21CS8122

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

char board[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
int player_sockets[2];
char* marks[2] = {"X", "O"};

void display_board() {
    printf("%c | %c | %c\n", board[0], board[1], board[2]);
    printf("--+---+--\n");
    printf("%c | %c | %c\n", board[3], board[4], board[5]);
    printf("--+---+--\n");
    printf("%c | %c | %c\n\n", board[6], board[7], board[8]);
}

int check_win(char mark) {
    int win_conditions[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
        {0, 4, 8}, {2, 4, 6}
    };

    for (int i = 0; i < 8; i++) {
        int a = win_conditions[i][0];
        int b = win_conditions[i][1];
        int c = win_conditions[i][2];
        if (board[a] == mark && board[b] == mark && board[c] == mark) {
            return 1;
        }
    }
    return 0;
}

void send_message(int socket, const char *message) {
    send(socket, message, strlen(message), 0);
}

int receive_move(int socket) {
    char buffer[BUFFER_SIZE];
    recv(socket, buffer, BUFFER_SIZE, 0);
    return atoi(buffer);
}

void play_game() {
    display_board();
    int turn = 0;

    while (1) {
        int player_index = turn % 2;
        int current_socket = player_sockets[player_index];
        char *mark = marks[player_index];

        for (int i = 0; i < 2; i++) {
            if (i == player_index) {
                send_message(player_sockets[i], mark);
                send_message(player_sockets[i], "Your turn: ");
            } else {
                send_message(player_sockets[i], "Waiting for the opponent's move...\n");
            }
        }

        int move = receive_move(current_socket);
        if (board[move] == ' ') {
            board[move] = mark[0];
            display_board();
            if (check_win(mark[0])) {
                int winner_index = player_index;
                int loser_index = (player_index + 1) % 2;
                send_message(player_sockets[winner_index], "Congratulations! You win!\n");
                send_message(player_sockets[loser_index], "You lose! Player ");
                send_message(player_sockets[loser_index], marks[winner_index]);
                send_message(player_sockets[loser_index], " wins.\n");
                sleep(2);
                break;
            } else if (strchr(board, ' ') == NULL) {
                for (int i = 0; i < 2; i++) {
                    send_message(player_sockets[i], "It's a draw!\n");
                }
                sleep(2);
                break;
            }
            turn++;
        } else {
            send_message(current_socket, "Invalid move! Try again.\n");
        }
    }
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 2) == -1) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for players...\n");

    for (int i = 0; i < 2; i++) {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) == -1) {
            perror("Accepting connection failed");
            exit(EXIT_FAILURE);
        }
        printf("Connected with %s\n", inet_ntoa(client_addr.sin_addr));
        player_sockets[i] = client_socket;
    }

    play_game();

    for (int i = 0; i < 2; i++) {
        close(player_sockets[i]);
    }
    close(server_socket);

    return 0;
}

