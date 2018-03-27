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

#define Server_Portnumber 4000
#define Server_IP_address 2130706433


int main(){
  int sockfd,new_fd; //socket file and connected file discripter

  struct sockaddr_in my_addr; //my address
  struct sockaddr_in their_addr; //connector address
  int sin_size;

  if((sockfd = socket(PF_INET, SOCK_STREAM,0)) == -1){ //create socket and save fd number
    perror("socket creating error");
    exit(1);
  }

  their_addr.sin_family = AF_INET;
  their_addr.sin_port = htons(Server_Portnumber);
  their_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  if(connect(sockfd, (struct sockaddr*)&their_addr, sizeof(struct sockaddr))== -1){
      perror("connect error");
      exit(1);
  }

}
