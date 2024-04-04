//Name:Pankaj Sah
//Roll No: 21CS8122
//Computer Networks Laboratory CSS652

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <arpa/inet.h>

#define MAX_PACKET_SIZE 1024

void error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int portno = 58122;

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char packet[MAX_PACKET_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("Error opening socket");

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        error("Error on binding");

    printf("Server listening on port %d...\n", portno);

    while (1) {
        client_len = sizeof(client_addr);
        int bytes_received = recvfrom(sockfd, packet, MAX_PACKET_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (bytes_received < 0)
            error("Error receiving data from client");

        packet[bytes_received] = '\0'; // Null-terminate the received data

        DIR *dir = opendir(packet);
        if (!dir) {
            // Send empty list to client if directory is not existent or not readable
            sendto(sockfd, "\0", 1, 0, (struct sockaddr *)&client_addr, client_len);
            continue;
        }

        int total_bytes_sent = 0;
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            int entry_len = strlen(entry->d_name);
            if (total_bytes_sent + entry_len + 2 > MAX_PACKET_SIZE) // Check if there's enough space in the packet
                break;

            strcpy(&packet[total_bytes_sent], entry->d_name);
            packet[total_bytes_sent + entry_len] = '\0'; // Null-terminate the file name
            total_bytes_sent += entry_len + 1; // Move the pointer to the next position
        }
        closedir(dir);

        // Add an extra null character to denote the end of the list
        packet[total_bytes_sent] = '\0';
        total_bytes_sent++;

        // Send the packet to the client
        sendto(sockfd, packet, total_bytes_sent, 0, (struct sockaddr *)&client_addr, client_len);
    }

    close(sockfd);

    return 0;
}

