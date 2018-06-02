#include "proxy.h"

void error(char * msg) {
   perror(msg);
   exit(0);
}

int exist_cache_file(char* name){
  int fd = open("cache_index",O_RDONLY);
  if(fd==-1)
    error("index file open error");

  char text[1000];

  int length;

  if((length = read(fd,text,1000))<0)
    error("index file read error");

  char * ptr = strtok(text, "\n");
  while(ptr){
    char addr[100],size[10],ts[50];
    sscanf(ptr,"%s %s %s",addr,size,ts);
    if(strcmp(name,addr)==0){
      int file_size = atoi(size);
      return file_size;
    }
    bzero(ptr, strlen(ptr));
    ptr = strtok(NULL, "\n");

  }
  return -1;
}

//Duction that returns the size of the headers in an HTTP message
int sizeHeaders(char * message) {
   char forwardHeader[100];
   int where = 0;

   char * aux = malloc(strlen(message) + 1);
   strcpy(aux, message);
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
   return where;
}

//Function that adds a Forwarded header to a http request
void addForwardHeader(char * clientaddress, char * serevraddres, char * message) {
   char forwardHeader[100];
   int where = 0;

   sprintf(forwardHeader, "Forwarded: for=%s; proto=http; by=%s\r\n\r\n", clientaddress, serevraddres);
   char * aux = malloc(strlen(message) + 1);
   strcpy(aux, message);
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

   message[where] = '\0';
   strcat(message, forwardHeader);
}

//Function that sends a 400 bad request response to the socket provided
void sendInvalidRequest(char * t3, int connfd) {
   char * message;
   strcpy(message, t3);
   strcat(message, " 400 Bad request\nConnection: Closed\nContent-Type: text/html; charset=UTF-8\r\n\r\n <h1>INVALID REQUEST</h1>");
   //printf("Sending the client %i bytes: \n%s\n",send(connfd, message, strlen(message), 0),message);
   send(connfd, message, strlen(message), 0);
}
// void check_validity_request()
//Function that returns in result the value of the headerTitle in a http request
//Returns true if the header is in the request and false if it isn't

bool get_header_value(char * index, char * value, char * commend) {
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
         return true;
      }
      bzero(ptr, strlen(ptr));
      ptr = strtok(NULL, "\n");
   }

   return false;
}


void logRequest(char * type, char * requestedObject, char * proto, char * responseCode, char * clientIP, char * responseSize) {
   FILE * fp = fopen("access.log", "a+");
   if (fp == NULL)
      error("Error with the log txt file\n");
   time_t t;
   time(&t);
   char log[300];

   //Getting the time
   sprintf(log, "%s ",strtok(ctime(&t),"\n"));
   strcat(log,"EST: ");
   strcat(log, clientIP);
   strcat(log, " ");
   strcat(log, requestedObject);
   strcat(log, " ");
   strcat(log, responseSize);
   strcat(log, "\n");
   //printf("TO log: %s\n",log);
   fprintf(fp, "%s", log);
   fclose(fp);
}

void logging(char * ip_adr, char * domain_adr,int file_size) {
   FILE * fp = fopen("proxy.log", "a+");
   if (fp == NULL)
      error("Error with the log txt file\n");
   time_t t;
   time(&t);
   char log[300];
   sprintf(log, "%s EST: %s %s %d\n",strtok(ctime(&t),"\n"),ip_adr,domain_adr,file_size);
   fprintf(fp, "%s", log);
   fclose(fp);
}


//Function that creates a simple HTTP request modifying the one received from the browser
//STRANGE ERROR: I created this function because in my linux virtual machine I got strange encoding
//errors when just sending the request from the browser (in the lab  pcs it worked fine so im not using
//this function)
void createHTTPrequest(char * message, char * request) {
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
}
