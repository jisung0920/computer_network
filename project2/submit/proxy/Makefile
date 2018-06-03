proxy : proxy_function.o cache_function.o proxy.o
		gcc -o proxy proxy_function.o cache_function.o proxy.o
proxy_function.o: proxy.h proxy_function.c
		gcc -c proxy_function.c
cache_function.o: proxy.h cache.h cache_function.c
		gcc -c cache_function.c
proxy.o : proxy.h proxy.c
		gcc -c proxy.c
clean :
		 rm *.o
