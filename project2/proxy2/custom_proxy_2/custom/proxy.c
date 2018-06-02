#include "proxy.h"

#define BUFFER_SIZE 1024

int main(int argc,char ** argv){
  if (argc < 2) {
       fprintf(stderr,"ERROR, no port provided\n");
       exit(1);
     }
  int proxy_sockfd, cli_sockfd,ser_sockfd;
  int PORT;

  char Buffer[BUFFER_SIZE],commend[BUFFER_SIZE];

  struct sockaddr_in proxy_addr, ser_addr, cli_addr;

  socklen_t clilen,proxylen,serlen;

  clilen = sizeof(cli_addr);

  //===============================set variable




  PORT = atoi(argv[1]);
  //===============================port check and allocate

  proxy_sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if(proxy_sockfd <0)
    error("socket open error");

  bzero((char*) &proxy_addr,sizeof(proxy_addr));

  proxy_addr.sin_family = AF_INET;
  proxy_addr.sin_addr.s_addr = INADDR_ANY;
  proxy_addr.sin_port = htons(PORT);

  //===============================server socket setting

  if(bind(proxy_sockfd,(struct sockaddr *) &proxy_addr, sizeof(proxy_addr)) <0 )
    error("socket bind error");

  listen(proxy_sockfd,LISTENQ);
  //===============================binding and queue setting


  fprintf(stdout, "%s\n","\nComputer Network - project 2\nProxy Server\n작성자 : 정지성" );

  while(1){

    int pid;
    int status;
    pid = fork();

    switch (pid) {

      case -1:{
        error("fork error");
      }
      //===============================fork error

      case 0 :{

        cli_sockfd = accept(proxy_sockfd, (struct sockaddr *) &cli_addr,&clilen);

        if(cli_sockfd < 0)
          error("accept error");

        char server_name[INET_ADDRSTRLEN],client_name[INET_ADDRSTRLEN];
        getsockname(cli_sockfd, (struct sockaddr *) &proxy_addr, & proxylen);
        inet_ntop(AF_INET, &proxy_addr, server_name, INET_ADDRSTRLEN);


        bzero(Buffer,BUFFER_SIZE);
        //===============================client socket setting and init buffer
        if (recv(cli_sockfd, Buffer, MAXLINE, 0) <= 0)
          break;

        char method[50], domain[100], protocol[100];

        printf("\nReceived from browser \n%s\n", Buffer);
        printf("server name : %s\n",server_name );
        sscanf(Buffer, "%s %s %s", method, domain, protocol);
        strcpy(commend,Buffer);
        //===============================check and print requested data

        struct addrinfo hints;
        struct addrinfo * result, * rp;
        int sfd;
        bzero(Buffer, BUFFER_SIZE);

        if (strcmp(method, "GET") != 0 && strcmp(method, "HEAD") != 0) {
           strcpy(Buffer, protocol);
           strcat(Buffer, " 405 Method not allowed\nConnection: Closed\nContent-Type: text/html; charset=UTF-8\r\n\r\n<h1>This proxy only work with HEAD and GET requests</h1>");

           send(cli_sockfd, Buffer, strlen(Buffer), 0);
           printf("Method error\nsend msg :\n'%s' \nto browser\n",Buffer);

        }

        char hostValue[200];
        get_header_value("Host", hostValue,commend);

        printf("Type: %s \nProtocol: %s \nWeb: %s\nHostName: %s\n\n", method, protocol, domain, hostValue);

        memset( & hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(hostValue, "http", & hints, & result) != 0) {
           printf("getaddrinfo() failed\n");
           sendInvalidRequest(protocol, cli_sockfd);
          //  logRequest(method, protocol, t3, "400", "", "0");==========

        }

        ser_sockfd = socket(result->ai_family,result->ai_socktype,result->ai_protocol);
        if(connect(ser_sockfd,result->ai_addr,result->ai_addrlen)<0)
          error("server connect error\n");

        inet_ntop(AF_INET, & (result->ai_addr), client_name, INET_ADDRSTRLEN);

        addForwardHeader(client_name,server_name,commend);


        bzero(Buffer,BUFFER_SIZE);
        createHTTPrequest(commend, Buffer);
        addForwardHeader(client_name, server_name, Buffer);
        printf("Sending to %s:\n%s\n", client_name, Buffer);
        send(ser_sockfd, Buffer, strlen(Buffer), 0);




        int cache_size = exist_cache_file(hostValue);
        if(cache_size!=-1){

          char path[100];
          sprintf(path,"./cache/%s",hostValue);
          int file = open(path,O_RDWR|O_CREAT|O_APPEND,0666);

          int n=0;
          bzero(Buffer, strlen(Buffer));
          while (n < cache_size+1) {
            //  naux = recv(ser_sockfd, Buffer, BUFFER_SIZE, 0);
            //  if(write(file,Buffer,naux)<0)
            //     printf("write error !!!\n\n" );
             int naux =read(file,Buffer,BUFFER_SIZE);
             send(cli_sockfd, Buffer, BUFFER_SIZE, 0);
             bzero(Buffer, BUFFER_SIZE);
             n = n + naux;
          }


        }
/*
//branch

        int recv_size, tmp_size = 0;

        recv_size = recv(ser_sockfd, Buffer, BUFFER_SIZE, 0);

        char headerValue[200];
        bzero(headerValue, 200);

        get_header_value("Content-Length", headerValue,Buffer);

        int sizePacket = atoi(headerValue) + sizeHeaders(Buffer);


        send(cli_sockfd, Buffer, recv_size, 0);

        bzero(Buffer, strlen(Buffer));
        while (recv_size < sizePacket) {
           tmp_size = recv(ser_sockfd, Buffer, BUFFER_SIZE, 0);
           send(cli_sockfd, Buffer, BUFFER_SIZE, 0);
           bzero(Buffer, BUFFER_SIZE);
           recv_size = recv_size + tmp_size;
        }
        printf("Sent a total of %i to the browser\n", recv_size);
*/
/*
        while (n < sizePacket) {
           naux = recv(ser_sockfd, Buffer, BUFFER_SIZE, 0);
           if(write(file,Buffer,naux)<0)
              printf("write error !!!\n\n" );
           printf("%s\n", Buffer);
           send(cli_sockfd, Buffer, BUFFER_SIZE, 0);
           bzero(Buffer, BUFFER_SIZE);
           n = n + naux;
        }

        printf("Sent a total of %i to the browser\n", n);
*/

        close(cli_sockfd);
        close(ser_sockfd);


      }
      default:{
        wait(&status);
      }
    }


  }

  close(cli_sockfd);
  // close(sockfd);

  return 0;
}
