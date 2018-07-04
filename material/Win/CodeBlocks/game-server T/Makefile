normal: server-threads.c
	gcc -w server-threads.c -o space-server -lpthread
verbose: server-threads.c
	gcc -D DEBUG server-threads.c -o space-server -lpthread
all: normal

clean: 
	rm -f space-server
