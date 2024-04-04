//Name: Pankaj Sah
//Roll No: 21CS8122

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include<sys/time.h>

#define PORT 58122
#define MAXLINE 1024

int isPalindrome(char *str) {
    int len = strlen(str);
    int start = 0,end = len-2;
    while(start <= end) {
        if (str[start] != str[end])
            return 0; // Not a palindrome
        start++;
        end--;
    }
    return 1; // Palindrome
}

int main() {
    int tcp_fd, udp_fd, max_fd, activity, sd, client_len, n;
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAXLINE];

    // Create TCP socket
    if ((tcp_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("TCP socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Create UDP socket
    if ((udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("UDP socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Clear server_addr structure
    memset(&server_addr, 0, sizeof(server_addr));

    // Initialize server_addr
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Bind TCP socket
    if (bind(tcp_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("TCP bind failed");
        exit(EXIT_FAILURE);
    }

    // Bind UDP socket
    if (bind(udp_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("UDP bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for TCP connections
    if (listen(tcp_fd, 5) == -1) {
        perror("TCP listen failed");
        exit(EXIT_FAILURE);
    }

    // Set max_fd
    max_fd = (tcp_fd > udp_fd) ? tcp_fd : udp_fd;

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(tcp_fd, &readfds);
        FD_SET(udp_fd, &readfds);

        // Use select to wait for any one of the two connections
        activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(tcp_fd, &readfds)) { // TCP connection
            sd = accept(tcp_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_len);
            if (sd < 0) {
                perror("TCP accept failed");
                exit(EXIT_FAILURE);
            }
            memset(buffer, 0, sizeof(buffer));
            n = read(sd, buffer, sizeof(buffer));
            if (n < 0) {
                perror("TCP read failed");
                exit(EXIT_FAILURE);
            }
            if (isPalindrome(buffer))
                strcpy(buffer, "Palindrome\n");
            else
                strcpy(buffer, "Not a palindrome\n");
            n = write(sd, buffer, strlen(buffer));
            if (n < 0) {
                perror("TCP write failed");
                exit(EXIT_FAILURE);
            }
            close(sd);
        }

        if (FD_ISSET(udp_fd, &readfds)) { // UDP connection
            memset(buffer, 0, sizeof(buffer));
            n = recvfrom(udp_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, (socklen_t *)&client_len);
            if (n < 0) {
                perror("UDP recvfrom failed");
                exit(EXIT_FAILURE);
            }
            if (isPalindrome(buffer))
                strcpy(buffer, "Palindrome\n");
            else
                strcpy(buffer, "Not a palindrome\n");
            n = sendto(udp_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
            if (n < 0) {
                perror("UDP sendto failed");
                exit(EXIT_FAILURE);
            }
        }
    }

    return 0;
}
