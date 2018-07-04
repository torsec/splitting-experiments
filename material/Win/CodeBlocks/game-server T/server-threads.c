#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#ifdef WIN32
   #include <windows.h>
   #include <winsock2.h>
#else
   #include <sys/socket.h>
   #include <netinet/in.h>
   #include <netinet/tcp.h>
#endif
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include "message-cache.h"

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define PORT 5001

// pointers to handle cache queue
struct Node* front = NULL;
struct Node* rear = NULL;

// variables to handle network connection
int sockfd, newsockfd, portno, clilen;
struct sockaddr_in serv_addr, cli_addr;

// variables to simulate client arguments
int argc;
char **argv;

// semaphores
#ifdef __APPLE__
sem_t *mutex, *elem_avail;
#else
sem_t mutex, elem_avail;
#endif

// thread to execute the slice
pthread_t slice;

// flag for main loop
int ACTIVE_EXEC = 1;

// function declarations
void process(int);
void return_value(int, size_t, size_t, size_t);
void *execute_slice();
int check_synch(int);
int check_answer(int, int);
int check_send(int);
void modify_the_cache_from_main(Message *, int);
void modify_the_cache_from_thread();
void initialize_argc(void *);
void initialize_argv(int);
void addToCache(Message *, size_t);
void deleteCacheEntry();
int isQueueEmpty();

//variables from the client
int row;
int col;
int direction;

enum{
	STOP=0,
	N   =1,
	NE  =2,
	E   =3,
	SE  =4,
	S   =5,
	SW  =6,
	W   =7,
	NW  =8
};

// main function
int main(void) {

#ifdef DEBUG
	printf("HANDLER: starting the server...\n");
#endif
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(1);
    }
	/* First call to socket() function */
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0) {
		perror("ERROR on socket");
		exit(1);
	}
#ifdef DEBUG
	printf("HANDLER: valid server socket opened, %d\n", sockfd);
#endif

	/* Initialize socket structure */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);

	/* sets socket options */
	int yes = 1;
	if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(int)) == -1) {
		perror("ERROR on setsockopt");
		exit(1);
	}
#ifdef DEBUG
	printf("HANDLER: socket options set\n");
#endif

	/* binds the current socket */
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		 perror("ERROR on bind");
		 exit(1);
	}
#ifdef DEBUG
	printf("HANDLER: binding on socket OK\n");
#endif

	/* starts listening on socket */
	if (listen(sockfd, 5) < 0) {
		perror("Error on listen");
		exit(1);
	}
#ifdef DEBUG
	printf("HANDLER: listening on socket OK\n");
#endif

	/* semaphore initialization */
#ifdef __APPLE__
	sem_unlink("mutex");
	sem_unlink("elem_avail");
	mutex = sem_open("mutex", O_CREAT, 0644, 1);
	elem_avail = sem_open("elem_avail", O_CREAT, 0644, 0);
#else
	sem_init (&mutex, 0, 1);
	sem_init (&elem_avail, 0, 0);
#endif

	/* checks for incoming connections */
	clilen = sizeof(cli_addr);
	while (1) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) {
			perror("ERROR on accept");
			exit(1);
		}
		else {
#ifdef DEBUG
			printf("HANDLER: connection OK\n");
#endif
			break;
		}
	}

	/* listens on the socket for incoming messages */
	while (ACTIVE_EXEC) {
#ifdef DEBUG
		printf ("HANDLER: calling PROCESS for incoming message\n");
#endif
		process (newsockfd);
    }

	/* closing all the resources */
    close (sockfd);
    close (newsockfd);
    WSACleanup();
#ifdef __APPLE__
    sem_close(mutex);
    sem_close(elem_avail);
    sem_unlink("mutex");
    sem_unlink("elem_avail");
#else
    sem_destroy (&mutex);
    sem_destroy (&elem_avail);
#endif

#ifdef DEBUG
    printf ("HANDLER: exiting..\n");
#endif
    exit (0);
}

// processes the incoming messages by reading on the socket
void process (int socket) {

#ifdef DEBUG
	printf ("PROCESS: reading incoming connection\n");
#endif

	/* reads len of current message from the socket */
	int len, err;
    recv (socket, &len, sizeof (int), 0);
#ifdef DEBUG
    printf ("PROCESS: reading message of len %d\n", len);
#endif

    /* processes the message if the len is consistent */
    if (len > 0) {

    	/* builds a copy of the message to be read from the socket */
        char buffer [256];
        bzero(buffer, 256);
        Message *entry = (Message *) buffer;
#ifdef DEBUG
        printf ("PROCESS: reading content of the current message\n");
#endif
        recv (socket, entry, len, 0);

        /* if message is not null, the content is processed*/
        if (entry != ((void *) 0)) {
        	/* checks the TYPE of the message to apply the proper action */
	    	switch(entry->TYPE) {

			case -1:
#ifdef DEBUG
				printf ("PROCESS: find stopping message\n");
#endif
				ACTIVE_EXEC = 0;
				break;

			case 1:
#ifdef DEBUG
				printf ("PROCESS: find slice message\n");
#endif
				err = pthread_create (&slice, 0, &execute_slice, ((void *) 0));
				if (err != 0) {
					perror("ERROR on pthread_create");
					exit(1);
				}
				break;

			case 2:
#ifdef DEBUG
				printf ("PROCESS: synchronizing SEND %zu\n", entry->LABEL);
#endif
                modify_the_cache_from_main (entry, len);
				break;

			case 3:
#ifdef DEBUG
				printf ("PROCESS: synchronizing SYNCH %zu\n", entry->LABEL);
#endif
                modify_the_cache_from_main (entry, len);
				break;

			case 4:
#ifdef DEBUG
				printf ("PROCESS: synchronizing ASK %zu with var request %zu\n", entry->LABEL, entry->VAR);
#endif
				modify_the_cache_from_main (entry, len);
				while (1) {
#ifdef __APPLE__
					sem_wait(mutex);
#else
					sem_wait (&mutex);
#endif
					if (isQueueEmpty () == 1)
						break;
#ifdef __APPLE__
					sem_post (mutex);
#else
					sem_post(&mutex);
#endif
				}
#ifdef __APPLE__
				sem_post (mutex);
#else
				sem_post(&mutex);
#endif
				return_value (socket, entry->VAR, entry->LABEL, entry->TYPE);
				break;

			case 5:
#ifdef DEBUG
				printf ("PROCESS: find message carrying argc\n");
#endif
                initialize_argc (entry->block);
				break;
			case 6:
#ifdef DEBUG
				printf ("PROCESS: find message carrying argv\n");
#endif
				initialize_argv (socket);
				break;

			default:
#ifdef DEBUG
				printf ("PROCESS: error in handling message..message not supported %zu\n", entry->TYPE);
#endif
				ACTIVE_EXEC = 0;
				break;
	    	}
        }
	}
}

// modifies the message queue from the main thread by adding entries
void modify_the_cache_from_main (Message *cache, int len) {
#ifdef __APPLE__
	sem_wait (mutex);
	addToCache (cache, len);
	sem_post (mutex);
	sem_post (elem_avail);
#else
	sem_wait (&mutex);
	addToCache (cache, len);
	sem_post (&mutex);
	sem_post (&elem_avail);
#endif
}

// modifies the message queue from the thread slice by deleting entries
void modify_the_cache_from_thread () {
#ifdef __APPLE__
	sem_wait (mutex);
	deleteCacheEntry ();
	sem_post (mutex);
#else
	sem_wait (&mutex);
	deleteCacheEntry ();
	sem_post (&mutex);
#endif
}

// returns requested values according to the variable asked
void return_value(int sock, size_t var, size_t label, size_t type){
	char conv[15];
	int len;
#ifdef DEBUG
	printf("PROCESS: answering to client request %zu.%zu\n", label, type);
#endif
	if (var == 1) {
		sprintf (conv, "%d", row);
		len = strlen (conv);
		send (sock, &len, sizeof (int), 0);
		send (sock, conv, len, 0);
	}
	if (var == 2) {
		sprintf (conv, "%d", col);
		len = strlen (conv);
		send (sock, &len, sizeof (int), 0);
		send (sock, conv, len, 0);
	}
}

// initializes argc global variable
void initialize_argc(void *arg) {
	argc = (int) arg;
}

// initializes argv global variable
void initialize_argv(int socket) {
	int len;
	recv (socket, &len, sizeof (int), 0);
	char str [len];
	recv (socket, str, len, 0);
	char local_str[len];
	strcpy(local_str, str);
	argv = malloc(argc * sizeof(char*));
	char * token = strtok(local_str, "@");
	int i = 0;
	while(token != ((void *)0)) {
		argv[i] = malloc((strlen(token) + 1) * sizeof(char));
		strcpy(argv[i], token);
		argv[i][strlen(token) + 1] = '\0';
		token = strtok(((void *)0),"@");
		i = i + 1;
	}
}

// checks the synchs sent by the client
int check_synch(int label) {
#ifdef DEBUG
	printf("CHECK_SYNCH: waiting for message %d\n", label);
#endif

	/* waits looping on the queue until the request is found */
	while(1) {
#ifdef __APPLE__
		sem_wait(elem_avail);
#else
		sem_wait(&elem_avail);
#endif
		if(front->message->TYPE == 3 && front->message->LABEL == label) {
			modify_the_cache_from_thread();
			return 1;
		}
		else
			usleep(1000);
	}
	return -1;
}

// checks the asks sent by the client
int check_answer(int var, int label) {
#ifdef DEBUG
	printf("CHECK_ANSWER: waiting for message %d\n", label);
#endif

	/* waits looping on the queue until the request is found */
	while(1) {
#ifdef __APPLE__
		sem_wait(elem_avail);
#else
		sem_wait(&elem_avail);
#endif
		if(front->message->TYPE == 4 && front->message->LABEL == label) {
			if (var == 1) {
				modify_the_cache_from_thread ();
				return row;
			}
			if (var == 2) {
				modify_the_cache_from_thread ();
				return col;
			}
		}
		else
			usleep(1000);
	}
	return -1;
}

int check_send(int label) {
#ifdef DEBUG
	printf("CHECK_SEND: waiting for message %d\n", label);
#endif

	/* waits looping on the queue until the request is found */
	while(1) {
#ifdef __APPLE__
		sem_wait(elem_avail);
#else
		sem_wait(&elem_avail);
#endif
		if(front->message->TYPE == 2 && front->message->LABEL == label) {
			switch(label) {
			  case 1 :
					{
						row = (int) front->message->block;
						break;
					}
				case 2 :
					{
						col = (int) front->message->block;
						break;
					}
				case 3 :
					{
						direction = (int) front->message->block;
						break;
					}
			}
			modify_the_cache_from_thread();
			return 0;
		}
		else
			usleep(1000);
	}
	return -1;
}

// adds an element to the cache
void addToCache(Message *entry, size_t bytes) {
	struct Node* temp = (struct Node*) malloc (sizeof(struct Node*));
	temp->message = (Message*) malloc (bytes);
	memcpy(temp->message, entry, bytes);

#ifdef DEBUG
	printf("ADD-TO-CACHE: %zu.%zu\n", temp->message->LABEL, temp->message->TYPE);
#endif

	if(front == ((void *)0) && rear == ((void *)0)) {
		front = rear = temp;
		return;
	}
	rear->next = temp;
	rear = temp;
}

// deletes elements from the cache
void deleteCacheEntry() {

	struct Node* temp = front;
	if(front == ((void *)0))
		return;
	if(front == rear)
		front = rear = ((void *)0);
	else
		front = front->next;

#ifdef DEBUG
	printf("DELETE-CACHE: %zu.%zu\n", temp->message->LABEL, temp->message->TYPE);
#endif

	free(temp);
}

// checks if the cache is empty
int isQueueEmpty(){
	if(front == ((void *)0) && rear == ((void *)0))
		return 1;
	else
		return 0;
}

void * execute_slice(){
	check_send(1);
	check_send(2);
	check_send(3);
	//criterion for conf 1, point->row, point->col
		switch(direction) {
		case N:
			row--;
			break;
		case NE:
			row--;
			col++;
			break;
		case E:
			col++;
			break;
		case SE:
			row++;
			col++;
			break;
		case S:
			row++;
			break;
		case SW:
			row++;
			col--;
			break;
		case W:
			col--;
			break;
		case NW:
			row--;
			col--;
			break;
		}
	check_answer(1, 1);
	check_answer(2, 2);
}
