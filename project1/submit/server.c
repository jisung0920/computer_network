#include "server_jisung.h"

int main(int argc, char *argv[])
{
    int sockfd, cli_sockfd;
    int PORT;
    int check;

    char Buffer[BUFFER_SIZE];

    struct sockaddr_in ser_addr, cli_addr;

    socklen_t clilen;

    clilen = sizeof(cli_addr);

    //===============================set variable



    if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
       }
    PORT = atoi(argv[1]);
    //===============================port check and allocate

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd <0)
      error("socket open error");

    bzero((char*) &ser_addr,sizeof(ser_addr));

    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = INADDR_ANY;
    ser_addr.sin_port = htons(PORT);

    //===============================server socket setting

    if(bind(sockfd,(struct sockaddr *) &ser_addr, sizeof(ser_addr)) <0 )
      error("socket bind error");

    listen(sockfd,10);
    //===============================binding and queue setting


    fprintf(stdout, "%s\n","\nComputer Network - project 1\nDefault file : index.html \n지원 가능 포멧 : html, jpeg, gif, pdf, mp3\n작성자 : 정지성" );

    while(1){

      int pid;
      int status;
      pid = fork();

      switch (pid) {

        case -1:{
          error("fork error");
          exit(1);
        }
        //===============================fork error

        case 0 :{

          cli_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr,&clilen);

          if(cli_sockfd < 0)
            error("accept error");

          bzero(Buffer,BUFFER_SIZE);
          //===============================client socket setting and init buffer


          if(read(cli_sockfd,Buffer,BUFFER_SIZE)==-1)
            error("read error");
          fprintf(stdout,"message  : %s",Buffer);
          //===============================check and print requested data


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

          close(fd);

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
