#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#define CHUNK 1024
void writefile(int sd)
{
char buf[CHUNK];int n,i;
FILE *file;
file = fopen("Output.txt", "w");
if(file==NULL)
{
printf("\n Error in opening a file");
}
while( (read(sd, buf, sizeof(buf))) > 0)
{
fputs(buf, file);
bzero(buf, sizeof(buf));
}
}
int main(int argc, char **argv)
{
int n;
int sd, port;
char buf[1024];
struct sockaddr_in server;
port=atoi(argv[1]);
/* create a stream socket */
if(( sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
{
fprintf(stderr, "can't create a socket\n");exit(1);
}
// bzero((char *)&server, sizeof(struct sockaddr_in));
server.sin_family = AF_INET;
server.sin_port = port;
server.sin_addr.s_addr = inet_addr("127.0.0.1");
/* connecting to the server */
if(connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1)
{
fprintf(stderr, "can't connect\n");
exit(1);
}
printf("\n Enter the command");
scanf("%s",buf);
/* get user's text */
write(sd, buf, sizeof(buf));
/* send it out */
//printf("\n\nEchoed Messege:\n**************\n");
// n = read(sd, buf, sizeof(buf));
// printf("%s\n\n\n",buf);
writefile(sd);
close(sd);
return(0);
}
