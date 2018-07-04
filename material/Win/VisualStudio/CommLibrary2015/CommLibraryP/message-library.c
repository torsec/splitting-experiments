/*
 * message-library.c
 *
 *  Created on: Apr 22, 2014
 *      Author: lestat
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
*/
#define WIN32
#ifdef WIN32
   #include <winsock2.h> 
   #include <windows.h>
#else
   #include <sys/types.h>
   #include <sys/socket.h>
   #include <netinet/in.h>
   #include <netinet/tcp.h>
   #include <netdb.h>
#endif
//#include <unistd.h>
#include "message-library.h"

// defines for handling connection data
#define PORT 5001
#define hostname "localhost"

// global variable to store socket info
int sockfd = -1;

#ifdef METRICS
int sent_messages = 0;
int received_messages = 0;
int payload_received = 0;
int payload_sent = 0;
int synch_messages = 0;
int send_messages = 0;
int ask_messages = 0;
#endif

// negotiates the connection
int handle_connection(){
	struct sockaddr_in address;
	struct hostent * host;
	int sock = -1;
	WSADATA wsa;
	
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
	/* create socket */
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock <= 0) {
		perror("ERROR on socket");
		return -1;
	}

	/* setting socket options */
	int yes = 1;
	if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(int)) == 1) {
		perror("ERROR on setsockopt");
		exit(1);
	}

	/* connect to server */
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	host = gethostbyname(hostname);
	if (! host) {
		perror("ERROR on determining the host");
		return -1;
	}

	memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);
	if (connect(sock, (struct sockaddr *)&address, sizeof(address))) {
		perror("ERROR on connect");
		return -1;
	}
	return sock;
}

// sends the bootstrap message to server
// calls handle_connection to establish connection
void send_initial_message() {

	/* attempts to connect */
	/* checks if connection has been established already */
	if( sockfd == -1) {
		sockfd = handle_connection();
		if(sockfd < 0) {
			fprintf(stderr, "ERROR on connecting the client to server\n");
			exit(1);
		}
	}
	/* creates the bootstrap message */
	char buffer[128];
	Message *msg = (Message *) buffer;
	msg->LABEL = -1;
	msg->TYPE = 1;
	msg->VAR = -1;
	msg->block = NULL;
	msg->numBytes = sizeof(msg->LABEL) + sizeof(msg->TYPE) + sizeof(msg->block) + sizeof(msg->VAR);

#ifdef DEBUG
	printf("CLIENT: about to send bootstrap message\n");
#endif

#ifdef METRICS
    send_messages++;
    sent_messages++;
    payload_sent = payload_sent + msg->numBytes;
#endif
    
    /* writes the current bootstrap message on the socket */
	int len = sizeof(Message);
	send(sockfd, &len, sizeof(int), 0);
	send(sockfd, msg, len, 0);
}

// sends a value to the server
void send_value_to_server(int LABEL, int value) {

	/* creates the send message */
	char buffer[128];
	Message *msg = (Message *) buffer;
	msg->LABEL = LABEL;
	msg->TYPE = 2;
	msg->VAR = -1;
	msg->block = (void *) value;
	msg->numBytes = sizeof(msg->LABEL) + sizeof(msg->TYPE) + sizeof(msg->block) + sizeof(msg->VAR);
    

#ifdef METRICS
    send_messages++;
    sent_messages++;
    payload_sent = payload_sent + msg->numBytes;
#endif

#ifdef DEBUG
    printf("CLIENT: message SEND %d is being delivered\n", LABEL);
#endif
    
    /* writes the current send message on the socket */
	int len = sizeof(Message);
	send(sockfd, &len, sizeof(int), 0);
	send(sockfd, msg, len, 0);
}

// sends a synchronization message to the server
void synch_with_server(int LABEL){

	/* creates the synch message */
	char buffer[128];
	Message *msg = (Message *) buffer;
	msg->LABEL = LABEL;
	msg->TYPE = 3;
	msg->VAR = -1;
	msg->block = NULL;
	msg->numBytes = sizeof(msg->LABEL) + sizeof(msg->TYPE) + sizeof(msg->block) + sizeof(msg->VAR);

#ifdef METRICS
    synch_messages++;
    sent_messages++;
    payload_sent = payload_sent + msg->numBytes;
#endif
    
#ifdef DEBUG
    printf("CLIENT: message SYNCH %d is being delivered\n", LABEL);
#endif

    /* writes the current synch message on the socket */
	int len = sizeof(Message);
	send(sockfd, &len, sizeof(int), 0);
	send(sockfd, msg, len, 0);
}

// asks a value to the server
// getting the value is performed in two steps
// first an ask message is sent, then server answer is received
int ask_value_from_server(int var, int LABEL){

	/* creates the ask message */
	char buffer[128];
	Message *msg = (Message *) buffer;
	msg->LABEL = LABEL;
	msg->TYPE = 4;
	msg->VAR = var;
	msg->block = NULL;
	msg->numBytes = sizeof(msg->LABEL) + sizeof(msg->TYPE) + sizeof(msg->block) + sizeof(msg->VAR);
    
#ifdef METRICS
    ask_messages++;
    sent_messages++;
    payload_sent = payload_sent + msg->numBytes;
#endif
    
#ifdef DEBUG
    printf("CLIENT: message ASK %d is being delivered\n", LABEL);
#endif

    /* writes the current ask message on the socket */
    int len = sizeof(Message);
	send(sockfd, &len, sizeof(int), 0);
	send(sockfd, msg, len, 0);

	/* prepares the variables to collect the answer */
	char * answer;
	int value = 0;

	/* reads the server answer from the socket */
	recv(sockfd, &len, sizeof(int), 0);
	if (len > 0) {
		answer = (char *)malloc((len+1)*sizeof(char));
		answer[len] = 0;
		recv(sockfd, answer, len, 0);

#ifdef METRICS
    received_messages++;
    payload_received = payload_received + sizeof(answer);
#endif

		value = atoi(answer);
		free(answer);
	}

#ifdef DEBUG
		printf("CLIENT: ASK %d carries value %d\n", LABEL, value);
#endif

	return value;
}

// sends length of argv to server in order to store them when required
void send_arglen_to_server(int number) {

	/* establish connection if needed */
	sockfd = handle_connection();
	if(sockfd < 0) {
		fprintf(stderr, "ERROR on connecting the client to server\n");
		exit(1);
	}

	/* creates the arglen message */
	char buffer[128];
	Message *msg = (Message *) buffer;
	msg->LABEL = 1;
	msg->TYPE = 5;
	msg->VAR = -1;
	msg->block = (void *) number;
	msg->numBytes = sizeof(msg->LABEL) + sizeof(msg->TYPE) + sizeof(msg->block) + sizeof(msg->VAR);
    
#ifdef METRICS
    send_messages++;
    sent_messages++;
    payload_sent = payload_sent + msg->numBytes;
#endif
    
#ifdef DEBUG
    printf("CLIENT: message ARGLEN is being delivered\n");
#endif

    /* writes the current arglen message on the socket */
	int len = sizeof(Message);
	send(sockfd, &len, sizeof(int), 0);
	send(sockfd, msg, len, 0);
}

// sends args to server
void send_args_to_server(int argn, char** args) {

	/* creates the args message */
	int sizeofargs = 0;
	int i, j;
	for(i = 0; i < argn; i++) {
		for (j = 0; args[i][j] != '\0'; j++) ;
		sizeofargs = sizeofargs + (j + 1);
	}

	char *string_args;

	string_args = (char*)malloc((sizeofargs + argn + 1)*sizeof(char));

	int last_char = 0;
	for(i = 0; i < argn; i++) {
		for (j = 0; args[i][j] != '\0'; j++)
			string_args[last_char + j] = args[i][j];

		string_args[last_char + j] = '@';
		last_char = last_char + j + 1;
	}

	string_args[last_char] = '\0';
	int str_len;
	str_len = strlen(string_args) + 1;

	/* packing the args in the message */
	char buffer[128];
	Message *msg = (Message *) buffer;
	msg->LABEL = 1;
	msg->TYPE = 6;
	msg->VAR = -1;
	msg->block = NULL;
	msg->numBytes = sizeof(msg->LABEL) + sizeof(msg->TYPE) + sizeof(msg->block) + sizeof(msg->VAR);
	int len = sizeof(Message);

#ifdef METRICS
    send_messages++;
    sent_messages++;
    payload_sent = payload_sent + msg->numBytes + sizeof(string_args);
#endif

#ifdef DEBUG
    printf("CLIENT: message ARGS is being delivered\n");
#endif

    /* writes the current args message on the socket */
	send(sockfd, &len, sizeof(int), 0);
	send(sockfd, msg, len, 0);
	send(sockfd, &str_len, sizeof(int), 0);
	send(sockfd, string_args, str_len, 0);
}

void send_last_message() {

	/* creates the end message */
	char buffer[128];
	Message *msg = (Message *) buffer;
	msg->LABEL = -1;
	msg->TYPE = -1;
	msg->VAR = -1;
	msg->block = NULL;
	msg->numBytes = sizeof(msg->LABEL) + sizeof(msg->TYPE) + sizeof(msg->block) + sizeof(msg->VAR);

#ifdef METRICS
    send_messages++;
    sent_messages++;
    payload_sent = payload_sent + msg->numBytes;
    printf("%d,%d,%d,%d,%d,%d,%d\n", sent_messages, received_messages, synch_messages, send_messages, ask_messages, payload_sent, payload_received);
#endif
    
#ifdef DEBUG
    printf("CLIENT: message END is being delivered\n");
#endif

    /* writes the current ending message on the socket */
	int len = sizeof(Message);
	send(sockfd, &len, sizeof(int), 0);
	send(sockfd, msg, len, 0);

	/* closes the socket */
	close(sockfd);
	WSACleanup();
}
