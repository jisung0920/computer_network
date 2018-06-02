#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
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

bool get_header_value(char * index, char * value, char * commend);
void error(char * msg);
bool exist_cache_file(char* name);
int sizeHeaders(char * message);
void addForwardHeader(char * clientaddress, char * serevraddres, char * message);
void sendInvalidRequest(char * t3, int connfd);
bool checkHeaderValue(char * message, char * headerTitle, char * result);
void logRequest(char * type, char * requestedObject, char * proto, char * responseCode, char * clientIP, char * responseSize);
void logging(char* ip_adr,char* domain_adr,int file_size);
void createHTTPrequest(char * message, char * request);
