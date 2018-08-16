 /**********************************************************************
 # File Name:   main.c
 # Version:     1.0
 # Mail:        shiyanhk@gmail.com 
 # Created Time: 2018-08-14	
 ************************************************************************/
#include <stdio.h>
#include "adlist.h"

int main(int argc,char **argv)
{
	list *stRedisList = malloc (sizeof(list));
	list *node1 = malloc(sizeof(list));
	list *node2 = malloc(sizeof(list));
	list *node3 = malloc(sizeof(list));
	stRedisList = listCreate();
	int *value1 = malloc(4);
	int *value2 = malloc(4);
	int *value3 = malloc(4);
	stRedisList = listAddNodeHead(node1,&value1);	
	stRedisList = listAddNodeHead(node2,&value2);	
	stRedisList = listAddNodeHead(node3,&value3);	
	listRelease(stRedisList);       
	listNode *node;
	printf("sizeof(*node)%d\n",sizeof(*node));


	/*
	typedef struct list_t {
		listNode *head;
		listNode *tail;
		void *(*dup)(void *ptr);
		void (*free)(void *ptr);
		int (*match)(void *ptr, void *key);
		int len;
	} list_t;

	list_t *list1  = malloc(sizeof(list_t));
	list1->head = list1->tail = NULL;
	list1->len = 0;
	list1->dup = NULL;
	list1->free = NULL;
	list1->match = NULL;

	void *ptr  = (void *)malloc(sizeof(int));
	
	int (*pfunc ) (int  *);

	
	pfunc = NULL;
	pfunc(ptr);
*/	
//simple dymanic string
#include "sds.h"

	sdsnewlen("redis", 5);

       return 0;
}

