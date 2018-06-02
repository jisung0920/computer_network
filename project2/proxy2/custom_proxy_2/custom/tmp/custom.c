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
           //printf("Sending the client %i bytes: \n%s\n",send(connfd, buffer, strlen(buffer), 0),buffer);

           send(cli_sockfd, Buffer, strlen(Buffer), 0);
           printf("Method error\nsend msg :\n'%s' \nto browser\n",Buffer);
          //  logRequest(protocol, t2copy, t3, "405", "", "0");
          //  goto closing;
          //break;========================
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

//branch

        int n, naux = 0;

        n = recv(ser_sockfd, Buffer, BUFFER_SIZE, 0);
        char t22[300], t33[10];
        sscanf(Buffer, "%s %s", t33, t22);
        char headerValue[200];
        bzero(headerValue, 200);
        //We check if the server response includes the content length header
        get_header_value("Content-Length", headerValue,Buffer);

        int sizePacket = atoi(headerValue) + sizeHeaders(Buffer);
        // printf("Checking Content-Length header: %s\nTotal size of the packet: %i\n",headerValue,sizePacket);

        //Logging the rquest with the size of it
        logRequest(method, domain, protocol, t22, client_name, headerValue);
        // logging(clientaddress,t2,sizePacket);
        //printf("%s\n",buffer);
        int file = open(hostValue,O_RDWR|O_CREAT|O_APPEND,0666);
      /*
        write(file,Buffer,n);

        send(cli_sockfd, Buffer, n, 0);
        */
        bzero(Buffer, strlen(Buffer));
        while (n < sizePacket) {
          //  naux = recv(ser_sockfd, Buffer, BUFFER_SIZE, 0);
          //  if(write(file,Buffer,naux)<0)
          //     printf("write error !!!\n\n" );
           naux =read(file,Buffer,BUFFER_SIZE);
           send(cli_sockfd, Buffer, BUFFER_SIZE, 0);
           bzero(Buffer, BUFFER_SIZE);
           n = n + naux;
        }


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
        close(file);
        close(cli_sockfd);
        close(ser_sockfd);

/*
        char *token;
        char *type;
        char *path;

        strtok(Buffer," ");
        token = strtok(NULL, " ");
        //===============================token setting

        type = search_type(token);

        path =find_path(Buffer,type,token);

        //===============================find filetype and filepath

        int fd;

        if((fd = open(path,O_RDONLY))!= -1){

          int size = find_f_size(fd);

          send_init_data(cli_sockfd, Buffer, type,size);

          send_data_to_client(fd, cli_sockfd,Buffer);

        }
        //===============================open file and send http format and requested data from file

        else
          error("file error");
*/
        close(cli_sockfd);

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
