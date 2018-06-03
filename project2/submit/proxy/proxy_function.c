#include "proxy.h"

void error(char * msg) {
   perror(msg);
   exit(0);
}

void send_method_error(int socket,char * protocol){
  char buffer[HEADERSIZE];
  strcpy(buffer,protocol);
  strcat(buffer, " 405 Method not allowed\nConnection: Closed\nContent-Type: text/html; charset=UTF-8\r\n\r\n<h1>This proxy only work with HEAD and GET requests</h1>");
  send(socket,buffer,HEADERSIZE,0);
  printf("Method error\nsend msg :\n'%s' \nto browser\n",buffer);
}

int send_cached_url(int client,Node* url){

  int object_size = url->object_size;
  char object[MAX_OBJECT_SIZE];
  strcpy(object,url->object);

  return send(client,object,object_size,0);
}

int deliver_object_server_to_client(int server,int client,char* object){

  int recv_size=0,
      send_size = 0,
      total_object_size = 0;

  char Buffer[MAX_OBJECT_SIZE];

  while(1){

    recv_size = recv(server, Buffer, MAX_OBJECT_SIZE, 0);
    printf("## Received object from server (size : %d)\n",recv_size);

    send_size = send(client, Buffer, recv_size, 0);
    printf("## Sent url to the browser (size : %d)\n", send_size);

    total_object_size += recv_size;
    strcat(object,Buffer);

    bzero(Buffer,recv_size);

    if(recv_size==0 || send_size==0)
      break;
  }

  return total_object_size;
}


//Function that sends a 400 bad request response to the socket provided
void send_invalid(char * t3, int connfd) {
   char * message;
   strcpy(message, t3);
   strcat(message, " 400 Bad request\nConnection: Closed\nContent-Type: text/html; charset=UTF-8\r\n\r\n <h1>INVALID REQUEST</h1>");
   printf("\n\n## Sent invalid request to client \n\n");
   send(connfd, message, strlen(message), 0);
}

int get_header_value(char * index, char * value, char * commend) {
   char * text = malloc(strlen(commend) + 1);
   strcpy(text, commend);
   char * ptr = strtok(text, "\n"); //parse at new line

   char tmp[50];

   while (ptr) { //while token was found
      if (strlen(ptr) == 1) {

         break;
      }
      if (strncmp(ptr, index, strlen(index)) == 0) {
         sscanf(ptr, "%s %s", tmp, value);
         char * ptr2 = strtok(value, ";");
         value[strlen(ptr2)] = '\0';
         return 1;
      }
      bzero(ptr, strlen(ptr));
      ptr = strtok(NULL, "\n");
   }

   return 0;
}


void logging(char * ip_adr, char * domain_adr,int file_size) {
   FILE * fp = fopen("proxy.log", "a+");
   if (fp == NULL)
      error("Error with the log txt file\n");
   time_t t;
   time(&t);
   char log[300];
   sprintf(log, "%s EST: %s %s %d\n",strtok(ctime(&t),"\n"),ip_adr,domain_adr,file_size);
   printf("### LOG : %s",log);
   fprintf(fp, "%s", log);
   fclose(fp);
}


void attach_foward_to_request(char * clientaddress, char * serevraddres, char * message, char * request) {
   char t1[300], t2[300], t3[10];
   char * temp = NULL;
   int flag = 0, i, port = 0;
   sscanf(message, "%s %s %s", t1, t2, t3);
   strcpy(t1, t2);

   flag = 0;

   for (i = 7; i < strlen(t2); i++) {
      if (t2[i] == ':') {
         flag = 1;
         break;
      }
   }

   temp = strtok(t2, "//");
   if (flag == 0) {
      port = 80;
      temp = strtok(NULL, "/");
   } else {
      temp = strtok(NULL, ":");
   }

   sprintf(t2, "%s", temp);
   //printf("host = %s", t2);
   if (flag == 1) {
      temp = strtok(NULL, "/");
      port = atoi(temp);
   }

   strcat(t1, "^]");
   temp = strtok(t1, "//");
   temp = strtok(NULL, "/");
   if (temp != NULL)
      temp = strtok(NULL, "^]");
   //printf("\npath = %s\nPort = %d\n", temp, port);

   bzero((char * ) request, sizeof(request));
   if (temp != NULL)
      sprintf(request, "GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n", temp, t3, t2);
   else
      sprintf(request, "GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n", t3, t2);

    char forwardHeader[100];
    int where =0;
    sprintf(forwardHeader, "Forwarded: for=%s; proto=http; by=%s\r\n\r\n", clientaddress, serevraddres);
    char * aux = malloc(strlen(request) + 1);
    strcpy(aux, request);
    char * ptr = strtok(aux, "\n"); //parse at new line

    while (ptr) { //while token was found
       //Blank line
       if (strlen(ptr) == 1) {
          break;
       }
       where = where + strlen(ptr) + 1;
       bzero(ptr, strlen(ptr));
       ptr = strtok(NULL, "\n");
    }
    request[where] = '\0';
    strcat(request, forwardHeader);
}
