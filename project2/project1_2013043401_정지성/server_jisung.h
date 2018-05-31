#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> //O_WRONLY
#include <unistd.h> //write(), close()
#include <sys/wait.h>

#define BUFFER_SIZE 1024


void error(char*);
char* search_type(char*);
int find_f_size(int);
void send_data_to_client(int,int,char*);
void send_init_data(int, char*, char*, int);
char* find_path(char* ,char* , char* );
