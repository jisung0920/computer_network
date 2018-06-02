#include "proxy.h"

void error(char * msg) {
   perror(msg);
   exit(0);
}
//function that counts lines in a txt file
int countLines(FILE * fp) {

   int lines = 0;
   char ch;

   while (!feof(fp)) {
      ch = fgetc(fp);
      if (ch == '\n') {
         lines++;
      }
   }
   rewind(fp);
   return lines;
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

//Function that returns in result the value of the headerTitle in a http request
//Returns true if the header is in the request and false if it isn't
bool checkHeaderValue(char * message, char * headerTitle, char * result) {
   char * aux = malloc(strlen(message) + 1);
   strcpy(aux, message);
   char * ptr = strtok(aux, "\n"); //parse at new line

   char aux2[50];

   while (ptr) { //while token was found
      //Blank line so we finished checking headers
      if (strlen(ptr) == 1) {
         break;
      }
      if (strncmp(ptr, headerTitle, strlen(headerTitle)) == 0) {
         sscanf(ptr, "%s %s", aux2, result);
         char * ptr2 = strtok(result, ";");
         result[strlen(ptr2)] = '\0';
         return true;
      }
      bzero(ptr, strlen(ptr));
      ptr = strtok(NULL, "\n");
   }

   return false;
}





//Function that logs a request
// void logRequest(char * type, char * requestedObject, char * proto, char * responseCode, char * clientIP, char * responseSize) {
//    FILE * fp = fopen("access.log", "a+");
//    if (fp == NULL)
//       error("Error with the log txt file\n");
//    time_t t = time(NULL);
//
//    char log[300];
//
//    //Getting the time
//    struct tm tm = * localtime( & t);
//    sprintf(log, "Date: %d-%d-%dT%d:%d:%d ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
//    strcat(log, clientIP);
//    strcat(log, " \"");
//    strcat(log, type);
//    strcat(log, " ");
//    strcat(log, requestedObject);
//    strcat(log, " ");
//    strcat(log, proto);
//    strcat(log, "\" ");
//    strcat(log, responseCode);
//    strcat(log, " ");
//    strcat(log, responseSize);
//    strcat(log, " Bytes\n\0");
//    //printf("TO log: %s\n",log);
//    fprintf(fp, "%s", log);
//    fclose(fp);
// }

// bool cache_check(char* domain_adr){
//     FILE *fp = fopen("cache",O_RDONLY);
//     int flag=0;
//
// }
// void send_cache_to_client(void){
//
// }

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

   //Getting the time
   sprintf(log, "%s EST: %s %s %d\n",strtok(ctime(&t),"\n"),ip_adr,domain_adr,file_size);
  //  strcat(log,"EST: ");
  //  strcat(log, ip_adr);
  //  strcat(log, " ");
  //  strcat(log, domain_adr);
  //  strcat(log, " ");
  //  strcat(log, file_size);
  //  strcat(log, "\n");
   //printf("TO log: %s\n",log);
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
