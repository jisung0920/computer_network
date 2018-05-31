server : server_function.o server.o
				gcc -o server server_function.o server.o

server_function.o : server_jisung.h server_function.c
				gcc -c server_function.c

server.o : server_jisung.h server.c
				gcc -c server.c

clean :
				rm *.o
