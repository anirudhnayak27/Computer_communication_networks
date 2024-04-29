#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define BUFLEN 1024 /* buffer length */
#define MAX_CLIENTS 10 /* Maximum number of clients */

// Structure to hold client information
struct client_info {
    int sd;
    struct sockaddr_in address;
};

// Function declaration for client handling thread
void *handle_client(void *arg);

// Array to store information about connected clients
struct client_info clients[MAX_CLIENTS];

int main(int argc, char **argv) {
    int sd, new_sd, client_len, n;
    struct sockaddr_in server, client;
    char buf[BUFLEN];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_port>\n", argv[0]);
        exit(1);
    }

    int yes = 1;
    int port = atoi(argv[1]);
    port = htons(port);

    // Initialize clients array
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].sd = -1;
    }

    /* create a stream socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "can't create a socket\n");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = port;
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    /* bind an address to the socket */
    if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        fprintf(stderr, "can't bind name to socket\n");
        exit(1);
    }

    /* queue up to 5 connect requests */
    listen(sd, 5);

    printf("Server waiting for clients...\n");

    while (1) {
        client_len = sizeof(client);
        if ((new_sd = accept(sd, (struct sockaddr *)&client, &client_len)) == -1) {
            fprintf(stderr, "can't accept client\n");
            exit(1);
        }
        printf("Client connected\n");

        // Add client to clients array
        int i;
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].sd == -1) {
                clients[i].sd = new_sd;
                clients[i].address = client;
                break;
            }
        }

        if (i == MAX_CLIENTS) {
            fprintf(stderr, "Too many clients. Connection rejected.\n");
            close(new_sd);
            continue;
        }

        // Create a new thread to handle this client
        pthread_t client_thread;
        struct client_info *client_info = (struct client_info *)malloc(sizeof(struct client_info));
        client_info->sd = new_sd;
        client_info->address = client;

        if (pthread_create(&client_thread, NULL, handle_client, (void *)client_info) != 0) {
            fprintf(stderr, "Error creating client thread\n");
            free(client_info);
            close(new_sd);
        }
    }

    close(sd);
    return 0;
}

// Function to handle individual client
void *handle_client(void *arg) {
    struct client_info *client_info = (struct client_info *)arg;
    int sd = client_info->sd;
    struct sockaddr_in client = client_info->address;
    char buf[BUFLEN];
    int n;

    while (1) {
        n = read(sd, buf, sizeof(buf));
        if (n == -1) {
            fprintf(stderr, "read error\n");
            break;
        } else if (n == 0) {
            printf("Client disconnected\n");
            break;
        }

        buf[n] = '\0';  // Null-terminate the received data
        printf("Client %s:%d: %s", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buf);

        // Send message to all clients except the sender
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].sd != -1 && clients[i].sd != sd) { // Ensure it's a valid client and not the sender
                if (write(clients[i].sd, buf, n) == -1) {
                    fprintf(stderr, "write error\n");
                    // Handle write error if needed
                }
            }
        }
        printf("Server: ");
        fgets(buf, BUFLEN, stdin);  
        if (strcmp(buf, "quit\n") == 0) {
            close(sd);
            exit(0);
        }
        
        // Send message to client
        if (write(sd, buf, strlen(buf)) == -1) {
            fprintf(stderr, "write error\n");
            exit(1);
        }
    }

    // Remove client from clients array
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sd == sd) {
            clients[i].sd = -1;
            break;
        }
    }

    // Close the client socket and free the memory
    close(sd);
    free(client_info);

    pthread_exit(NULL);
}
