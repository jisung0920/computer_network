#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define PORT 4000
#define BACKLOG 10

#define Server_IP_address "127.0.0.1"



int main(){

  int sockfd,new_fd; //socket file and connected file discripter

  struct sockaddr_in my_addr; //my address
  struct sockaddr_in their_addr; //connector address

/*  struct sockaddr_in{
    socket sin_family; //address family
    unsigned short sin_port; //ip port
    struct in_addr sin_addr; //ip address
    char sin_zero[8]; //sockaddr 와 구조체 전체크기를 맞추기 위함
  }*/

/*  struct in_addr{
    u_long s_addr; //32bit ip address
}*/


  int sin_size;

  if((sockfd = socket(PF_INET, SOCK_STREAM,0)) == -1){ //create socket and save fd number
    //socket( domain, type, protocol)
    //PF_INET - ipv4 (protocol system)  cf) AF_INET ipv4 (address system) , PF_INET6 -ipv6, PF_LOCAL
    //SOCK_STREAM = TCP based
    perror("socket creating error");
    exit(1);
  }

  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(PORT);
  my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

//socket address setting

  if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))==-1){
    perror("bind error");
    exit(1);
  }// register socket as server

  if(listen(sockfd,BACKLOG) == -1){
    perror("listen error");
    exit(1);
  }
  while(1){
    sin_size = sizeof(struct sockaddr_in);
    if((new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size)) == -1){
      perror("accept error");
      continue;
    }
    printf("server : got connection from %s\n",inet_ntoa(their_addr.sin_addr) );
  }
}
