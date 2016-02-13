#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    int val;
    struct node * next;
} node_t;

int main(){
	char *name = "John";
	char num[10];
	strcpy(num, name);
/*	node_t * head = NULL;
	head = malloc(sizeof(node_t));
	if (head == NULL) {
	    return 1;
	}

	head->val = 2;
	head->next = NULL;*/
/*	int aInt = 368;
	char str[15];
	snprintf(str, 15, "%d", aInt);*/
	printf("%s\n", num);
}