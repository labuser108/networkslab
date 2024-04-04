#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rsocket.h"

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[50];

    // Create MRP socket
    sockfd = r_socket(SOCK_MRP, 0, 0);
    if (sockfd < 0) {
        perror("r_socket");
        exit(EXIT_FAILURE);
    }

    // Bind to a port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(52002); // Replace with your port number

    if (r_bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("r_bind");
        exit(EXIT_FAILURE);
    }

    printf("Enter a string (25 < length < 50): ");
    fgets(buffer, sizeof(buffer), stdin);

    int len = strlen(buffer);
    if (len < 26 || len > 49) {
        printf("Invalid string length. Exiting...\n");
        exit(EXIT_FAILURE);
    }

    // Send each character as a separate message
    for (int i = 0; i < len - 1; i++) {
        if (r_sendto(sockfd, &buffer[i], 1, 0, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
            perror("r_sendto");
            exit(EXIT_FAILURE);
        }
    }

    r_close(sockfd);
    return 0;
}
