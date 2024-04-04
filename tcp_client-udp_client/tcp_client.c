//Name:Pankaj Sah
//RollNo:21CS8122

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 58122
#define MAXLINE 1024

int main() {
    int sockfd, n;
    char buffer[MAXLINE];
    struct sockaddr_in server_addr;

    // Create TCP socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("TCP socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    // Initialize server_addr
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("TCP connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Enter a string: ");
    fgets(buffer, MAXLINE, stdin);

    // Send data to server
    write(sockfd, buffer, strlen(buffer));

    // Receive response from server
    n = read(sockfd, buffer, sizeof(buffer));
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);

    close(sockfd);

    return 0;
}
