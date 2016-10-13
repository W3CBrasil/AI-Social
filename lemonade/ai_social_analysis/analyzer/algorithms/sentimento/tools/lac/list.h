#ifndef _LIST_
#define _LIST_

typedef struct list_node {
	char* id;
	int label, size, *instance;
	list_node* next;
} list_t;

#endif
