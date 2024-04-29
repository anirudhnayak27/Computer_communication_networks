#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFLEN 1024 /* buffer length */

int main(int argc, char **argv) {
    int sd, port, n;
    char buf[BUFLEN];
    struct sockaddr_in server;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(1);
    }

    port = atoi(argv[2]);
    port = htons(port);

    /* create a stream socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "can't create a socket\n");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = port;
    server.sin_addr.s_addr = inet_addr(argv[1]);

    /* connecting to the server */
    if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        fprintf(stderr, "can't connect\n");
        exit(1);
    }

    printf("Connected to server. Start chatting...\n");

    while (1) {
        printf("Host: ");
        fgets(buf, BUFLEN, stdin);
        if (write(sd, buf, strlen(buf)) == -1) {
            fprintf(stderr, "write error\n");
            exit(1);
        }
        if (strcmp(buf, "quit\n") == 0) {
                close(sd);
                exit(0);
            }

        n = read(sd, buf, sizeof(buf));
        if (n == -1) {
            fprintf(stderr, "read error\n");
            exit(1);
        } else if (n == 0) {
            printf("Server closed connection\n");
            break;
        }

        buf[n] = '\0';  // Null-terminate the received data
        printf("Server: %s", buf);
    }

    close(sd);
    return 0;
}