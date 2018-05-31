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
void logRequest(char * type, char * requestedObject, char * proto, char * responseCode, char * clientIP, char * responseSize) {
   FILE * fp = fopen("access.log", "a+");
   if (fp == NULL)
      error("Error with the log txt file\n");
   time_t t = time(NULL);

   char log[300];

   //Getting the time
   struct tm tm = * localtime( & t);
   sprintf(log, "Date: %d-%d-%dT%d:%d:%d ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
   strcat(log, clientIP);
   strcat(log, " \"");
   strcat(log, type);
   strcat(log, " ");
   strcat(log, requestedObject);
   strcat(log, " ");
   strcat(log, proto);
   strcat(log, "\" ");
   strcat(log, responseCode);
   strcat(log, " ");
   strcat(log, responseSize);
   strcat(log, " Bytes\n\0");
   //printf("TO log: %s\n",log);
   fprintf(fp, "%s", log);
   fclose(fp);
}

//Not  used!
void removeLine(char * message) {
   int i, max, newlinePosition;
   char * aux = malloc(strlen(message));
   strcpy(aux, message);

   for (i = 0; i < strlen(message);)
      i++;
   newlinePosition = i;
   max = strlen(message) - newlinePosition;
   for (i = 0; i < max; i++) {
      message[i] = aux[newlinePosition];
      newlinePosition++;
   }
   message[newlinePosition + 1] = '\0';
}

// Function that modifies a webName so it's in the correct format for getaddrsinfo()
//It is also used when checking if a web is banned
void setCleanName(char * webNameRaw) {
   int i, lengthx;
   char * ptr = strtok(webNameRaw, "/"); //parse at new line
   char * ptr2;

   while (ptr) { //while token was found
      if (strncmp(ptr, "www", 3) == 0) {
         break;
      }
      ptr = strtok(NULL, "/");
   }

   char * aux = malloc(strlen(ptr));
   strcpy(aux, ptr);
   ptr2 = strtok(aux, ":");
   bzero(webNameRaw, strlen(webNameRaw));
   for (i = 0; i < strlen(ptr2); i++)
      webNameRaw[i] = ptr[i];
   webNameRaw[i] = '\0';
}

//Function that returns whether a webname is within the a list of blocked websites
bool isBanned(char * * bannedSites, char * hostNameRaw, int numberWebsBanned) {
   int i;
   //struct hostent *host1=gethostbyname(webSite);

   //cleaning host name
   setCleanName(hostNameRaw);

   if (!hostNameRaw)
      return false;
   for (i = 0; i < numberWebsBanned; i++) {
      if (strcmp(bannedSites[i], hostNameRaw) == 0) {
         printf("%s is banned\n", hostNameRaw);
         return true;
      }
   }
   return false;
}

//Function that gets a list of blocked websites from a filename
//It returns the numbers of sites in the list
int getBannedNames(char * * banned, char * fileName) {
   //Loading the banned websites
   FILE * bannedWebs = fopen(fileName, "r");
   int i;

   if (bannedWebs == NULL) {
      printf("Error opening %s\n", fileName);
      exit(1);
   }
   int nfiles = countLines(bannedWebs);
   for (i = 0; i < nfiles; ++i) {
      banned[i] = malloc(300); /* allocate a memory slot of 128 chars */
      fscanf(bannedWebs, "%1299s", banned[i]);
      printf("Banned web number %i: %s\n", i, banned[i]);
   }
   printf("\n\n");
   return nfiles;
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
