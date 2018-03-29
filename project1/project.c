#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> //O_WRONLY
#include <unistd.h> //write(), close()
#include <sys/wait.h>

#define BUFFER_SIZE 1024

void error(char*);
char* search_type(char*);
int find_f_size(int);

int main(int argc, char *argv[])
{
    int sockfd, cli_sockfd; //descriptors rturn from socket and accept system calls
    int PORT; // port number
    int check;

    char rBuffer[BUFFER_SIZE];
    char wBuffer[BUFFER_SIZE];
     /*sockaddr_in: Structure Containing an Internet Address*/

    struct sockaddr_in ser_addr, cli_addr;

    socklen_t clilen;

    clilen = sizeof(cli_addr);


    if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
       }
    PORT = atoi(argv[1]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd <0)
      error("socket open error");

    bzero((char*) &ser_addr,sizeof(ser_addr));

    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = INADDR_ANY;
    ser_addr.sin_port = htons(PORT);

    if(bind(sockfd,(struct sockaddr *) &ser_addr, sizeof(ser_addr)) <0 )
      error("socket bind error");

    listen(sockfd,10);


    while(1){
      int pid;
      int status;
      pid = fork();

      switch (pid) {
        case -1:{
          error("fork error");
          exit(1);
        }
        case 0 :{
          cli_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr,&clilen);

          if(cli_sockfd < 0)
            error("accept error");

          bzero(rBuffer,BUFFER_SIZE);

          check = read(cli_sockfd,rBuffer,BUFFER_SIZE);
          if(check==-1)
            error("read error");

          fprintf(stdout,"message  : %s",rBuffer);
//-----------------------------------
          char *token;
          char *content_type;
          char *path;

          strtok(rBuffer," ");
          token = strtok(NULL, " ");

          content_type = search_type(token);

          if(!strcmp(content_type,"em")){
            path = (char *)malloc(15);
            path = "./index.html";
            content_type = "text/html";

          }
          else{
            path = (char *)malloc(strlen(token)+1);
            sprintf(path,".%s",token);
          }

          int fd;
          if((fd = open(path,O_RDONLY))!= -1){
            int i=0;
            int size = find_f_size(fd);
            memset(wBuffer,'\0',BUFFER_SIZE);
            sprintf(wBuffer,"%s%s%s%d%s%s", "HTTP/1.1 200 OK\r\nContent-Type: ", content_type, "\r\nContent-Length: ", size, "\r\nConnection: Keep - Alive", "\r\n\r\n");
            fprintf(stdout, "%s\n",wBuffer );

            check = write(cli_sockfd, wBuffer,strlen(wBuffer));
            if(check==-1)
              error("write error");

            memset(rBuffer, '\0',BUFFER_SIZE);
            memset(wBuffer, '\0',BUFFER_SIZE);

            while((size=read(fd,rBuffer,BUFFER_SIZE))>0)
              write(cli_sockfd,rBuffer,BUFFER_SIZE);
            close(fd);
          }
          else
            error("file error");

        }
        default:{
          wait(&status);
        }
      }


    }

    close(cli_sockfd);
    close(sockfd);

    return 0;
}

int find_f_size(int fd){
    int size = lseek(fd, 0, SEEK_END); //file의 크기를 저장
    lseek(fd, 0, SEEK_SET); //file가리키는 위치 초기화

    return size;
}

char* search_type(char* token){

    char* arr[] = {".html", ".gif",".jpeg", ".mp3", ".pdf"};
    char *content_type[] = {"text/html", "image/gif", "image/jpeg", "audio/mpeg", "application/pdf"};
    char* res = "em";
    char* n;

    int i;
    for(i = 0; i < 5; i++){
        if((n = strstr(token, arr[i])) != NULL){
            res = content_type[i];
        }
    }
    return res;
}
void error(char *msg)
{
    perror(msg);
    exit(1);
}
