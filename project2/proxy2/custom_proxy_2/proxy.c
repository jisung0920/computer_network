
#include "proxy.h"




//Definition of boolean value

int main(int argc, char * * argv) {
   int listenfd, connfd;
   pid_t pid;
   struct sockaddr_in servaddr, cli_addr, proxy_addr;
   int clilen = sizeof(cli_addr);
   int proxylen = sizeof(proxy_addr);
   int sizePacket;

   if (argc != 2) {
      printf("usage: <Port>\n");
      exit(1);
   }//인자 받아오기 체크

   //Creation of the TCP socket
   listenfd = socket(AF_INET, SOCK_STREAM, 0);
   if (listenfd < 0)
      error("Error initializing socket (socket())\n");
   bzero( & servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
   servaddr.sin_port = htons(atoi(argv[1]));
   //Servers port is binded to the socket

   if (bind(listenfd, (SA * ) & servaddr, sizeof(servaddr)) < 0)
      error("Error binding socket (bind())\n");
   //Socket converted into a listening socket that will accept incoming connections
   //ListenQ defines the maximum number of connection s in line

   if (listen(listenfd, LISTENQ) < 0)
      error("Error on listening");

   printf("**** Proxy server up and running**** \n\nCreated by Luis Serra Garcia\n");

   //받아온 인자파일위치로 함수 실행
   for (;;) {
      accepting://??

         connfd = accept(listenfd, (SA * ) & cli_addr, & clilen);
      if (connfd < 0) {
         printf("Error accepting connection\n");
         continue;
      }

      char serevraddres[INET_ADDRSTRLEN];
      getsockname(connfd, (SA * ) & proxy_addr, & proxylen);
      inet_ntop(AF_INET, & (proxy_addr), serevraddres, INET_ADDRSTRLEN);

      pid = fork();
      if (pid == 0) {
         char bufferx[MAXLINE + 1];
         char buffer[MAXLINE + 1];
         char t1[300], t2copy[300], t2[300], t3[10];
         char client_command[MAXLINE + 1];

         bzero(client_command, strlen(client_command));

         if (recv(connfd, client_command, MAXLINE, 0) <= 0)
            goto closing;
         printf("Received from browser \n%s\n", client_command);
         sscanf(client_command, "%s %s %s", t1, t2, t3);
         strcpy(t2copy, t2);

         struct addrinfo hints;
         struct addrinfo * result, * rp;
         int sfd;
         bzero(buffer, MAXLINE + 1);

         if (strcmp(t1, "GET") != 0 && strcmp(t1, "HEAD") != 0) {
            strcpy(buffer, t3);
            strcat(buffer, " 405 Method not allowed\nConnection: Closed\nContent-Type: text/html; charset=UTF-8\r\n\r\n<h1>This proxy only work with HEAD and GET requests</h1>");
            //printf("Sending the client %i bytes: \n%s\n",send(connfd, buffer, strlen(buffer), 0),buffer);
            printf("Method error\n");
            send(connfd, buffer, strlen(buffer), 0);
            logRequest(t1, t2copy, t3, "405", "", "0");
            goto closing;
         }//요청이  GET HEAD 가 아니면 브라우져에게 잘못되었다고 보냄


//----------------------

         char hostName[200];
         checkHeaderValue(client_command, "Host", hostName);

         printf("Type: %s \nProtocol: %s \nWeb: %s\nHostName: %s\n\n", t1, t3, t2, hostName);

         memset( & hints, 0, sizeof(struct addrinfo));
         hints.ai_family = AF_UNSPEC;
         hints.ai_socktype = SOCK_STREAM;

         if (getaddrinfo(hostName, "http", & hints, & result) != 0) {
            printf("getaddrinfo() failed\n");
            sendInvalidRequest(t3, connfd);
            logRequest(t1, t2copy, t3, "400", "", "0");
            goto closing;
         }

         for (rp = result; rp != NULL; rp = rp->ai_next) {
            sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (sfd == -1)
               continue;

            if (connect(sfd, rp->ai_addr, rp->ai_addrlen) == -1) {
               printf("Connect error\n");
               close(sfd);
               continue;
            }
            break; // if we get here, we must have connected successfully
         }

           char clientaddress[INET_ADDRSTRLEN];
         inet_ntop(AF_INET, & (rp->ai_addr), clientaddress, INET_ADDRSTRLEN);

         if (rp == NULL) {
            printf("Could not bind to the desired server\n");
            sendInvalidRequest(t3, connfd);
            logRequest(t1, t2copy, t3, "400", "", "0");
            goto closing;
         }

         freeaddrinfo(result); // No longer needed

         addForwardHeader(clientaddress, serevraddres, client_command);


         bzero((char * ) bufferx, sizeof(bufferx));
         createHTTPrequest(client_command, bufferx);
         addForwardHeader(clientaddress, serevraddres, bufferx);
         printf("Sending to %s:\n%s\n", t2, bufferx);
         send(sfd, bufferx, strlen(bufferx), 0);

         int n, naux = 0;

         n = recv(sfd, buffer, MAXLINE, 0);
         char t22[300], t33[10];
         sscanf(buffer, "%s %s", t33, t22);
         char headerValue[200];
         bzero(headerValue, 200);
         //We check if the server response includes the content length header
         checkHeaderValue(buffer, "Content-Length", headerValue);

         //If the length of the headerValue is 0 means that the response didn't include a content-length header
         //Sending everything that the remote server sends
         if (strlen(headerValue) == 0) {
            send(connfd, buffer, strlen(buffer), 0);
            bzero(buffer, strlen(buffer));
            while (naux = recv(sfd, buffer, MAXLINE, 0) > 0) {
              //  printf("Received: %s\n", buffer);
               send(connfd, buffer, MAXLINE, 0);
               bzero(buffer, MAXLINE + 1);
               n = n + naux;
            }
            char auxSize[300];
            sprintf(auxSize, "%i", n);
            //logging the request with the size of it
            printf("Sent a total of %i to the browser\n", n);
            logRequest(t1, t2, t3, t22, clientaddress, auxSize);
         }

         //In this case the proxy will have the size of the http response so it can make sure that it sends everything
         else {
            //The size of the packet is the content-length header plus the size of the headers
            sizePacket = atoi(headerValue) + sizeHeaders(buffer);
            // printf("Checking Content-Length header: %s\nTotal size of the packet: %i\n",headerValue,sizePacket);

            //Logging the rquest with the size of it
            logRequest(t1, t2, t3, t22, clientaddress, headerValue);
            logging(clientaddress,t2,sizePacket);
            //printf("%s\n",buffer);

            send(connfd, buffer, n, 0);
            bzero(buffer, strlen(buffer));
            while (n < sizePacket) {
               naux = recv(sfd, buffer, MAXLINE, 0);
              //  printf("%s\n", buffer);
               send(connfd, buffer, MAXLINE, 0);
               bzero(buffer, MAXLINE + 1);
               n = n + naux;
            }

            printf("Sent a total of %i to the browser\n", n);
         }
         //------------------------
         closing:
            bzero(client_command, MAXLINE + 1);
         close(connfd);
         close(sfd); //It is done by the remote server so is not needec
         //Terminating the child
         exit(0);
      } else {
         //Parent
         close(connfd);
      }
   }
   close(listenfd);
}
