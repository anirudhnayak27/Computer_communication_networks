#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#define BUFLEN 1024 /* buffer length */
int main(int argc, char **argv)
{
 int n;
int yes=1;
 int sd, new_sd, client_len, port;
 struct sockaddr_in server, client;
 char buf[BUFLEN];
 port = atoi(argv[1]);

 /* create a stream socket */
 if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
 {
 fprintf(stderr,"can't create a socket\n");
 exit(1);
 }
 /* Fill the structure fileds with values */
 server.sin_family = AF_INET;
 server.sin_port = port;
 server.sin_addr.s_addr =inet_addr("127.0.0.1");
 // Reuse the port and address
/*if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
 perror("setsockopt");
 exit(1);
}
*/
 /* bind an address to the socket */
 if(bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1)
 {
 fprintf(stderr, "can't bind name to socket\n");
 exit(1);
 }
 /* queue up to 5 connect requests */
 listen(sd,5);
 while(1)
 {
 client_len = sizeof(client);
 if((new_sd = accept(sd, (struct sockaddr *) &client, &client_len)) == -1)
 {
 fprintf(stderr, "can't accept client \n");
 exit(1);
 }
 n = read(new_sd, buf, sizeof(buf));
printf("The message received by client : %s\n",buf);
 write(new_sd, buf,n);
close(new_sd);
 }
 close(sd);
 return(0);
}