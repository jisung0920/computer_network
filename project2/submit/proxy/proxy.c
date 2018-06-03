#include "proxy.h"
#include "cache.h"


int main(int argc,char ** argv){

  if (argc < 2)
      error("PORT is not provided\nusage './proxy PORTNUMBER'");

  int proxy_sockfd,
      cli_sockfd,
      ser_sockfd;

  int PORT;

  char Buffer[MAX_BUF_SIZE],
       commend[MAX_BUF_SIZE];

  struct sockaddr_in proxy_addr,
                     ser_addr,
                     cli_addr;

  socklen_t clilen,proxylen,serlen;

  clilen = sizeof(cli_addr);

  LinkedList* cache_list;
  cache_list = clinit();

  //===============================set variable

  fprintf(stdout, "%s\n","\nComputer Network - project 2\nProxy Server (Hanyang univ. Computer Network 2018 spring)\n작성자 : 정지성" );


  PORT = atoi(argv[1]);

  proxy_sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if(proxy_sockfd <0)
    error("socket open error");

  bzero((char*) &proxy_addr,sizeof(proxy_addr));

  proxy_addr.sin_family = AF_INET;
  proxy_addr.sin_addr.s_addr = INADDR_ANY;
  proxy_addr.sin_port = htons(PORT);

  if(bind(proxy_sockfd,(struct sockaddr *) &proxy_addr, sizeof(proxy_addr)) <0 )
    error("socket bind error");

  listen(proxy_sockfd,LISTENQ);
  sleep(1);

  fprintf(stdout, "\n%s\n","\n## proxy server setting complete\n##  listening" );

  //===============================proxy server socket setting



  while(1){

    int pid;
    int status;

    pid = fork();

    switch (pid) {
      //===============================fork

      case -1:{
        error("fork error");
      }

      case 0 :{

        p_cache_list(cache_list);

        char method[L_METHOD],
             domain[L_DOMAIN],
             protocol[L_PROTOCOL],
             client_name[INET_ADDRSTRLEN],
             server_name[INET_ADDRSTRLEN];

        Node* cached_url;

        cli_sockfd = accept(proxy_sockfd, (struct sockaddr *) &cli_addr,&clilen);

        if(cli_sockfd < 0)
          error("accept error");

        fprintf(stdout, "\n%s\n","## Browser request to proxy" );


        getsockname(cli_sockfd, (struct sockaddr *) &proxy_addr, & proxylen);
        inet_ntop(AF_INET, &proxy_addr, server_name, INET_ADDRSTRLEN);

  //===============================client setting


        bzero(Buffer,MAX_BUF_SIZE);
        if (recv(cli_sockfd, Buffer, MAXLINE, 0) <= 0){
          printf("\n\n## Browser request to proxy error.\n");
          break;
        }

        printf("\n\n## Received from browser \n********\n%s\n********\nserver : %s\n", Buffer,server_name);

        sscanf(Buffer, "%s %s %s", method, domain, protocol);


        if (strcmp(method, "GET") != 0 && strcmp(method, "HEAD") != 0)
           send_method_error(cli_sockfd,protocol);

  //===============================handle received msg from the browser



        cached_url = search(cache_list,domain);

        if(cached_url != NULL){

          int cached_size = send_cached_url(cli_sockfd,cached_url);

          printf("\n\n## Log request to 'proxy.log' file\n");
          logging(client_name,domain,cached_size);

          //===============================log request


          printf("## Send cached url to the browser (size : %d) \n",cached_size);
        }
  //===============================send cached data to the browser


        else{

          char hostValue[L_HOSTVALUE],
               object[MAX_OBJECT_SIZE];

          int init_object_size = 0,
              object_size = 0,
              total_object_size = 0;

          struct addrinfo hints;
          struct addrinfo * result;


          strcpy(commend,Buffer);

          get_header_value("Host", hostValue,commend);

          printf("Type: %s \nProtocol: %s \nWeb: %s\nHostName: %s\n\n", method, protocol, domain, hostValue);

          printf("\n\n## Connecting with server...\n");

          memset( & hints, 0, sizeof(struct addrinfo));
          hints.ai_family = AF_UNSPEC;
          hints.ai_socktype = SOCK_STREAM;

          if (getaddrinfo(hostValue, "http", & hints, & result) != 0) {
             printf("getaddrinfo() failed\n");
             send_invalid(protocol, cli_sockfd);
          }

          ser_sockfd = socket(result->ai_family,result->ai_socktype,result->ai_protocol);

          if(connect(ser_sockfd,result->ai_addr,result->ai_addrlen)<0)
            error("server connect error\n");

          inet_ntop(AF_INET, & (result->ai_addr), client_name, INET_ADDRSTRLEN);

          printf("\n\n## Server connect complete\n");

  //===============================server setting


          bzero(Buffer,MAX_BUF_SIZE);
          attach_foward_to_request(client_name, server_name, commend, Buffer);

          //===============================attach header


          init_object_size = send(ser_sockfd, Buffer, strlen(Buffer), 0);
          printf("\n\n## Sent request to %s(server) from %s(client) by proxy server\n********\n%s\n********\n", server_name,client_name ,Buffer);



          object_size = deliver_object_server_to_client(ser_sockfd,cli_sockfd,object);

          total_object_size = init_object_size + object_size;

          printf("\n\n## Log request to 'proxy.log' file\n");
          logging(client_name,domain,total_object_size);

          //===============================log request

          if(total_object_size<MAX_OBJECT_SIZE){
            add(cache_list,nodeinit(domain,object,total_object_size));
            printf("\n\n## Caching object (name : %s)\n",domain );
            bzero(object,MAX_OBJECT_SIZE);
          }
          else{
            printf("\n\n## Object size is %d\n### Cannot cache (over MAX_OBJECT_SIZE)\n",total_object_size);
          }
        }
  //===============================deliver msg to the browser from server

      }

      default:{
        wait(&status);
      }
    }
    bzero(Buffer, strlen(Buffer));
    close(cli_sockfd);
    close(ser_sockfd);

  }

  close(cli_sockfd);
  close(ser_sockfd);

  return 0;
}
