#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rsocket.h"

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[1];

    // Create MRP socket
    sockfd = r_socket(SOCK_MRP, 0, 0);
    if (sockfd < 0) {
        perror("r_socket");
        exit(EXIT_FAILURE);
    }

    // Bind to a port
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(52003); // Replace with your port number

    if (r_bind(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        perror("r_bind");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int n = r_recvfrom(sockfd, buffer, 1, 0, NULL, NULL);
        if (n < 0) {
            perror("r_recvfrom");
            exit(EXIT_FAILURE);
        } else if (n > 0) {
            printf("%c", buffer[0]);
            fflush(stdout);
        }
    }

    r_close(sockfd);
    return 0;
}
