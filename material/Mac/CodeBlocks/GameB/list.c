#include "list.h" 
#include <stdlib.h> 
static LinkedList* createNode(void* data);
static void freeNode(LinkedList* node);

List* createList(){
	List* newList= (List*) malloc(sizeof(List));
	assert(newList);
	newList->start = NULL;
	newList->end = NULL;
	return newList;
}
void destroyList(List* list){
	LinkedList* node;
	LinkedList* nodeTmp;
	node = list->start;
	while(node != NULL){
		nodeTmp = node;
		node = node->next;
		removeNode(list, nodeTmp);
	}
	list->start = NULL;
	list->end = NULL;
}
int contains(List* list, void* data){
	LinkedList* node;
	for(node= list->start; node != NULL; node= node->next){
		if(data == node->data){
			return 1;
		}
	}
	return 0;
}
void addData(List* list, void* data){
	LinkedList* newNode = createNode(data);
	LinkedList* lastNode;
	assert (newNode != NULL);
	if(list->start == NULL){
		list->start = newNode;
	}else{
		lastNode = list->end;
		newNode->prev = lastNode; 
		lastNode->next = newNode;
	}
	list->end = newNode;
}
void removeData(List* list, void* data){
	LinkedList *node, *nodeTmp;
	node = list->start;
	while(node){
		if(node->data == data){
			nodeTmp = node;
			node = nodeTmp->next;
			removeNode(list, nodeTmp);
		}else{
			node = node->next;
		}
	}
}
void removeNode(List* list, LinkedList* node){
	if(node->prev != NULL)
		node->prev->next = node->next;
	if(node->next != NULL)	
		node->next->prev = node->prev;
	if(list->start == node){
		list->start =node->next;
	}
	if(list->end == node){
		list->end = node->prev;
	}
	freeNode(node);
}

// Private metods
static LinkedList* createNode(void* data){
	LinkedList* newNode = (LinkedList*) malloc(sizeof(LinkedList));
	if(newNode != NULL){
		newNode->data = data;
		newNode->next= NULL;
		newNode->prev= NULL;
	}
	return newNode;
}
static void freeNode(LinkedList* node){
	free(node);
}

