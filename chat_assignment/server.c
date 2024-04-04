//Name- Pankaj Sah
//Roll No- 21CS8122

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 50000
#define MAX_CLIENTS 5
#define MAX_MESSAGE_LENGTH 200
#define MAX_NAME_LENGTH 50
#define MAX_BUFFER_SIZE 256

typedef struct {
    int sockfd;
    char name[MAX_NAME_LENGTH];
    struct sockaddr_in address;
    int addr_len;
    pthread_t thread_id;
} ClientInfo;

ClientInfo clients[MAX_CLIENTS];
int num_clients = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void send_to_all_clients(char *message, int sender_index) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_clients; i++) {
        if (i != sender_index) {
            send(clients[i].sockfd, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&mutex);
}

void *handle_client(void *arg) {
    int client_index = *((int *)arg);
    char buffer[MAX_BUFFER_SIZE];
    while (1) {
        int bytes_received = recv(clients[client_index].sockfd, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            printf("Client %s disconnected\n", clients[client_index].name);
            close(clients[client_index].sockfd);
            pthread_mutex_lock(&mutex);
            for (int i = client_index; i < num_clients - 1; i++) {
                clients[i] = clients[i + 1];
            }
            num_clients--;
            pthread_mutex_unlock(&mutex);
            break;
        } else {
            buffer[bytes_received] = '\0';
            if (strlen(buffer) > MAX_MESSAGE_LENGTH) {
                send(clients[client_index].sockfd, "Message too long\n", 17, 0);
            } else {
                struct timeval tv;
                gettimeofday(&tv, NULL);
                long current_time = tv.tv_sec;
                char time_str[20];
                strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
                char formatted_message[MAX_BUFFER_SIZE];
                sprintf(formatted_message, "[%s %s] %s", time_str, clients[client_index].name, buffer);
                send_to_all_clients(formatted_message, client_index);
            }
        }
    }
    free(arg);
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Chat server started on port %d\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&mutex);
        if (num_clients >= MAX_CLIENTS) {
            send(new_socket, "Too many users, please try after some time\n", 45, 0);
            close(new_socket);
            pthread_mutex_unlock(&mutex);
            continue;
        }

        // Receive client name
        char client_name[MAX_NAME_LENGTH];
        int bytes_received = recv(new_socket, client_name, sizeof(client_name), 0);
        if (bytes_received <= 0) {
            perror("recv");
            close(new_socket);
            pthread_mutex_unlock(&mutex);
            continue;
        }
        client_name[bytes_received] = '\0';

        // Add client to array
        clients[num_clients].sockfd = new_socket;
        strcpy(clients[num_clients].name, client_name);
        clients[num_clients].address = address;
        clients[num_clients].addr_len = addrlen;

        // Create thread to handle client
        int *arg = malloc(sizeof(*arg));
        if (arg == NULL) {
            fprintf(stderr, "Error allocating memory\n");
            exit(EXIT_FAILURE);
        }
        *arg = num_clients;
        pthread_create(&clients[num_clients].thread_id, NULL, handle_client, arg);

        num_clients++;
        pthread_mutex_unlock(&mutex);

        printf("Client %s connected\n", client_name);
    }

    return 0;
}

