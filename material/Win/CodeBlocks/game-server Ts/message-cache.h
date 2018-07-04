/*
 * message-cache.h
 *
 *  Created on: Apr 4, 2014
 *      Author: lestat
 */

#ifndef MESSAGE_QUEUE_H_
#define MESSAGE_QUEUE_H_

typedef struct {
	size_t LABEL;
    size_t TYPE;
    size_t VAR;
    size_t numBytes;
    void *block;
} Message;

struct Node {
	Message *message;
	struct Node* next;
};

#endif /* MESSAGE_CACHE_H_ */

