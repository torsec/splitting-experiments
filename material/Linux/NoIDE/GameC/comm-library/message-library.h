#ifndef MESSAGE_LIBRARY_H_
#define MESSAGE_LIBRARY_H_

// defines struct Message
typedef struct {
	size_t LABEL;
	size_t TYPE;
	size_t VAR;
	size_t numBytes;
	void * block;
} Message;

int handle_connection();
void synch_with_server(int);
void send_value_to_server(int, int);
void send_last_message();
void send_initial_message();
void send_arglen_to_server(int);
void send_args_to_server(int, char **);
int ask_value_from_server(int, int);

#endif /* MESSAGE_LIBRARY_H_ */
