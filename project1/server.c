/* 
   A simple server in the internet domain using TCP
   Usage:./server port (E.g. ./server 10000 )
*/
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
char* search_type(char*); //type에 따라서 content_type을 return 하는 함수
int getFileSize(int); // file size를 return 해주는 함수

int main(int argc, char *argv[])
{
    int sockfd, newsockfd; //descriptors rturn from socket and accept system calls
    int portno; // port number
    socklen_t clilen;
     
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
     /*sockaddr_in: Structure Containing an Internet Address*/
    struct sockaddr_in serv_addr, cli_addr;
     
    int n;
    if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     
     /*Create a new socket
       AF_INET: Address Domain is Internet 
       SOCK_STREAM: Socket Type is STREAM Socket */
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]); //atoi converts from String to Integer
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY; //for the server the IP address is always the address that the server is running on
     serv_addr.sin_port = htons(portno); //convert from host to network byte order
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //Bind the socket to the server address
              error("ERROR on binding");
     
     listen(sockfd,5); // Listen for socket connections. Backlog queue (connections to wait) is 5
     fprintf(stdout,"\n\n");
     clilen = sizeof(cli_addr);
     /*accept function: 
       1) Block until a new connection is established
       2) the new socket descriptor will be used for subsequent communication with the newly connected client.
     */
     while(1){
        int pid;
        int status;

        pid = fork();

        if(pid == -1){
            error("fork error");
            exit(1);
        } 
        if(pid == 0){
            newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

            if (newsockfd < 0)
                error("ERROR on accpet");
            bzero(buffer, BUFFER_SIZE);

            if (read(newsockfd, buffer, BUFFER_SIZE) == -1)
                error("ERROR on read");
            fprintf(stdout, "### request message ###\n");
            fprintf(stdout, "%s", buffer);

            char *token;
            char *content_type;
            char *path;

            strtok(buffer, " ");
            token = strtok(NULL, " ");         //token 값 잘라서 파일명 구분
            content_type = search_type(token); //response 할 때 content_type을 적어주기 위해서 씀

            if (!strcmp(content_type, "em"))
            { // 만약 ip 주소만 있을 경우, 없는 파일일 경우에는 무조건 index.html로 가게 함
                path = (char *)malloc(15);
                path = "./index.html";
                content_type = "text/html";
            }
            else
            {
                path = (char *)malloc(strlen(token) + 1);
                sprintf(path, ".%s", token);
            }
            int fd;
            if ((fd = open(path, O_RDONLY)) != -1)
            {
                int i = 0;
                int size = getFileSize(fd);
                memset(response, '\0', BUFFER_SIZE);
                sprintf(response, "%s%s%s%d%s%s", "HTTP/1.1 200 OK\r\nContent-Type: ", content_type, "\r\nContent-Length: ", size, "\r\nConnection: Keep - Alive", "\r\n\r\n");
                fprintf(stdout, "\n### response message ###\n");
                fprintf(stdout, "%s\n", response);

                write(newsockfd, response, strlen(response)); //client(newsockfd)에게 response message길이 만큼 write한다.

                memset(buffer, '\0', BUFFER_SIZE);

                while ((size = read(fd, buffer, BUFFER_SIZE)) > 0)
                { //open한 File을 Buffer의 크기만큼 읽어서 계속 write한다.
                    write(newsockfd, buffer, BUFFER_SIZE);
                }
                close(fd);
            }
            else
            {
                error("No File in SERVER");
            }

            fprintf(stdout, "######################################################\n\n");
        }else{
            wait(&status);
        }
     }
     close(newsockfd);
     close(sockfd);

     return 0; 
}
int getFileSize(int fd){
    int size = lseek(fd, 0, SEEK_END); //file의 크기를 저장
    lseek(fd, 0, SEEK_SET); //file가리키는 위치 초기화

    return size;
}

char* search_type(char* token){
    char* arr[] = {".html", ".gif",".jpeg", ".mp3", ".pdf",".ico"};
    char *content_type[] = {"text/html", "image/gif", "image/jpeg", "audio/mpeg", "application/pdf", "image/x-icon"};
    char* res = "em";
    char* temp;
    int i;
    for(i = 0; i < 6; i++){
        if((temp = strstr(token, arr[i])) != NULL){
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
