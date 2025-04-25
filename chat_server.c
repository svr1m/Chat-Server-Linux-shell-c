#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    struct sockaddr_in address;
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg) {
    Client client = *(Client *)arg;
    free(arg);
    char buffer[BUFFER_SIZE];
    int bytes_received;
    char message[BUFFER_SIZE + 50]; // Extra space for client identifier

    printf("Client [%s:%d] connected.\n",
           inet_ntoa(client.address.sin_addr),
           ntohs(client.address.sin_port));

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client.socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                // Connection closed by the client
                printf("Client [%s:%d] disconnected.\n",
                       inet_ntoa(client.address.sin_addr),
                       ntohs(client.address.sin_port));
            } else {
                // Error occurred during recv
                perror("recv");
            }

            // Remove client from list and close socket
            pthread_mutex_lock(&mutex);
            for (int i = 0; i < client_count; ++i) {
                if (clients[i].socket == client.socket) {
                    clients[i] = clients[--client_count];
                    break;
                }
            }
            pthread_mutex_unlock(&mutex);
            close(client.socket);
            pthread_exit(NULL);
        }

        buffer[bytes_received] = '\0'; // Null-terminate the message

        // Create message with client identifier
        snprintf(message, sizeof(message), "Client [%s:%d] says: %s",
                 inet_ntoa(client.address.sin_addr),
                 ntohs(client.address.sin_port),
                 buffer);

        // Broadcast message to all clients
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < client_count; ++i) {
            if (clients[i].socket != client.socket) {
                send(clients[i].socket, message, strlen(message), 0);
            }
        }
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", atoi(argv[1]));

    // Accept incoming connections and handle each in a separate thread
    while (1) {
        Client *new_client = malloc(sizeof(Client));
        new_client->socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (new_client->socket < 0) {
            perror("Error accepting connection");
            free(new_client);
            continue;
        }
        new_client->address = client_addr;

        pthread_mutex_lock(&mutex);
        if (client_count < MAX_CLIENTS) {
            clients[client_count++] = *new_client;
            pthread_mutex_unlock(&mutex);

            pthread_t tid;
            pthread_create(&tid, NULL, handle_client, new_client);
            pthread_detach(tid);  // Detach the thread to free resources when it finishes
        } else {
            pthread_mutex_unlock(&mutex);
            close(new_client->socket);
            free(new_client);
            printf("Maximum clients connected. Connection rejected.\n");
        }
    }

    close(server_socket);

    return 0;
}

