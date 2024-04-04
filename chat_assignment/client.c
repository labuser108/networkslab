//Name-Pankaj Sah
//Roll No - 21CS8122
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 50000
#define MAX_MESSAGE_LENGTH 200

void *receive_messages(void *sock_fd) {
    int sockfd = *((int *)sock_fd);
    char buffer[MAX_MESSAGE_LENGTH + 32]; // Extra space for timestamp and sender name
    while (1) {
        int bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            printf("Server disconnected\n");
            close(sockfd);
            exit(EXIT_FAILURE);
        } else {
            buffer[bytes_received] = '\0';
            printf("%s\n", buffer);
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("invalid address");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    // Send client name
    char name[50];
    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';
    send(sockfd, name, strlen(name), 0);

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, receive_messages, &sockfd);

    char message[MAX_MESSAGE_LENGTH];

    // Main loop to send messages
    while (1) {
        printf("Enter message: ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';

        if (strlen(message) > MAX_MESSAGE_LENGTH) {
            printf("Message too long. Please enter a shorter message.\n");
            continue;
        }

        send(sockfd, message, strlen(message), 0);
    }

    return 0;
}

