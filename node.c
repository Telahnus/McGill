#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    int val;
    struct node * next;
} node_t;

int main(){
	node_t * head = malloc(sizeof(node_t));
	node_t * tail = malloc(sizeof(node_t));

	head->val = 2;
	head->next = tail;
/*	int aInt = 368;
	char str[15];
	snprintf(str, 15, "%d", aInt);*/
	//printf("%s\n", num);
}