#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <time.h>
#include <arpa/inet.h>

#define MAXLINE 4096
#define LISTENQ 1024
#define L_PROTOCOL 20
#define L_METHOD 50
#define L_DOMAIN 100
#define L_HOSTVALUE 200
#define HEADERSIZE 8192
#define MAX_BUF_SIZE 1024
#define MAX_OBJECT_SIZE 524288
#define MAX_CACHE_SIZE 5242880

typedef struct Node {
    time_t timestemp;
    char url[MAX_BUF_SIZE];
    char object[MAX_OBJECT_SIZE];
    int object_size;
    struct Node *next;
} Node;

typedef struct LinkedList {
    int remainder_size;
    struct Node *header;
} LinkedList;

void error(char * msg);
void send_method_error(int socket,char * protocol);
int send_cached_url(int client,Node* url);
int deliver_object_server_to_client(int server,int client,char* object);
int get_header_value(char * index, char * value, char * commend);
void attach_foward_to_request(char * clientaddress, char * serevraddres, char * message, char * request);
void logging(char* ip_adr,char* domain_adr,int file_size);
void send_invalid(char * protocol, int client);
