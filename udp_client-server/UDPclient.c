//Name:Pankaj Sah
//Roll No: 21CS8122
//Computer Networks Laboratory CSS652

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_PATH_LENGTH 256
#define MAX_PACKET_SIZE 1024

void error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(stderr, "Usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char server_ip[] = "127.0.0.1"; // Server IP address
    int server_port = 58122;

    int sockfd;
    struct sockaddr_in server_addr;
    char path[MAX_PATH_LENGTH];
    char packet[MAX_PACKET_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("Error opening socket");

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    printf("Enter the absolute path of the directory: ");
    fgets(path, MAX_PATH_LENGTH, stdin);
    strtok(path, "\n"); // Remove newline character

    int path_len = strlen(path);

    if (sendto(sockfd, path, path_len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        error("Error sending data to server");

    socklen_t server_len = sizeof(server_addr);
    int bytes_received = recvfrom(sockfd, packet, MAX_PACKET_SIZE, 0, (struct sockaddr *)&server_addr, &server_len);
    if (bytes_received < 0)
        error("Error receiving data from server");

    printf("List of files in the directory:\n");
    int i = 0;
    while (i < bytes_received) {
        printf("/0%s\n", &packet[i]);
        i += strlen(&packet[i]) + 1; // Move to the next null-terminated string
    }

    close(sockfd);

    return 0;
}

