#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <time.h>
#define MAXLINE 4096
#define SA struct sockaddr
#define maxThreads 100
#define LISTENQ 1024
typedef int bool;
enum {
   false,
   true
};
void error(char * msg);
int countLines(FILE * fp);
int sizeHeaders(char * message);
void addForwardHeader(char * clientaddress, char * serevraddres, char * message);
void sendInvalidRequest(char * t3, int connfd);
bool checkHeaderValue(char * message, char * headerTitle, char * result);
void logRequest(char * type, char * requestedObject, char * proto, char * responseCode, char * clientIP, char * responseSize);
void removeLine(char * message);
void setCleanName(char * webNameRaw);
bool isBanned(char * * bannedSites, char * hostNameRaw, int numberWebsBanned);
int getBannedNames(char * * banned, char * fileName);
void createHTTPrequest(char * message, char * request);
