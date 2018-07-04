#include <stdio.h>
#include <assert.h>
/*
	USAGE:
	First create the list:
		List* list;
		list = createList();
	then add data:
		addData(list, data);
	or remove data:	
		removeData(list, data);

	finally destroy list:
		destroyList(list);
*/


typedef struct LinkedList LinkedList;
struct LinkedList{
	LinkedList* next;
	void * data;
	LinkedList* prev;
};

typedef struct List List;
struct List{
	LinkedList* start;
	LinkedList* end;
};


List* createList();
void destroyList(List* list);
int contains(List* list, void* data);
void addData(List* list, void* data);
void removeData(List* list, void* data);
void removeNode(List* list, LinkedList* node);
