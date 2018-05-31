#include "server_jisung.h"
void send_init_data(int client, char* data, char* type, int length){

  memset(data,'\0',BUFFER_SIZE);

  sprintf(data,"%s%s%s%d%s%s", "HTTP/1.1 200 OK\r\nContent-Type: ", type, "\r\nContent-Length: ", length, "\r\nConnection: Keep - Alive", "\r\n\r\n");

  if( write(client, data,strlen(data))==-1)
    error("write error");

  fprintf(stdout, "%s\n",data );

}//input: client socket, requested file type and length   function: send http fomat to client

void send_data_to_client(int fd, int client,char* data){

  memset(data, '\0',BUFFER_SIZE);

  while((read(fd,data,BUFFER_SIZE))>0)
    write(client,data,BUFFER_SIZE);

}//input: requested file fd, client socket  function: send data to client

int find_f_size(int fd){

    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    return size;
}//input: fd    output: size

char* search_type(char* token){

    char* ext[] = {".html", ".gif",".jpeg", ".mp3", ".pdf"};
    char *type[] = {"text/html", "image/gif", "image/jpeg", "audio/mpeg", "application/pdf"};
    char* fileType = "em";

    for(int i = 0; i < 5; i++){
        if(strstr(token, ext[i]) != NULL){
            fileType = type[i];
        }
    }
    return fileType;
}//input: filetoken(string)   output: file type


char* find_path(char* data,char* type, char* token){
  char* path;

  if(!strcmp(type,"em")){
    path = (char *)malloc(15);
    path = "./index.html";
    type = "text/html";

  }
  else{
    path = (char *)malloc(strlen(token)+1);
    sprintf(path,".%s",token);
  }

  return path;
}//input: data(string), requested data type, filetoken   output: file path


void error(char *msg)
{
    perror(msg);
    exit(1);
}
