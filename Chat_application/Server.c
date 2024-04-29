#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFLEN 1024 /* buffer length */

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

        while (1) {
            n = read(new_sd, buf, sizeof(buf));
            if (n == -1) {
                fprintf(stderr, "read error\n");
                exit(1);
            } else if (n == 0) {
                printf("Client disconnected\n");
                close(new_sd);
                break;
            }

            buf[n] = '\0';  // Null-terminate the received data
            printf("Client: %s", buf);

            printf("Server: ");
            fgets(buf, BUFLEN, stdin);  
            if (strcmp(buf, "quit\n") == 0) {
                close(new_sd);
                close(sd);
                exit(0);
            }
            
            // Send message to client
            if (write(new_sd, buf, strlen(buf)) == -1) {
                fprintf(stderr, "write error\n");
                exit(1);
            }
        }
    }

    close(sd);
    return 0;
}