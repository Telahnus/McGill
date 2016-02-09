/**************************************************
FILE: CircularBuffer.c
DESCRIPTION: circular queue/buffer, this implementation
	does not allow overflow/underflow.
AUTHOR: Jordan Quan
LAST REVISED: 2014/10/15
**************************************************/

// includes
#include <stdio.h>
#include <stdlib.h>

// typedef struct
typedef struct {
    int 	first;
    int     last;
    int     count;
    int 	size;
    int     *data;
} CircularBuffer;

// prototypes
void 	cbInit		(CircularBuffer *cb, int size);		// initialize cb with size
void 	cbWrite		(CircularBuffer *cb, int val);		// write to last free spot in cb
void 	cbPrint		(CircularBuffer *cb);				// print all elements of cb
void 	cbDestroy	(CircularBuffer *cb);				// free all memory of cb
int 	cbIsEmpty	(CircularBuffer *cb);				// 1 if cb is empty
int 	cbRead		(CircularBuffer *cb);				// return oldest data in cb
int 	cbFirst		(CircularBuffer *cb);				// return  oldest index
int 	cbLast		(CircularBuffer *cb);				// return latest index written to

void cbInit(CircularBuffer *cb, int size)
{
    int i;
    cb->size 	= size;
    cb->count 	= 0;
    cb->first 	= 0;
    cb->last 	= 0;
    cb->data = (int*)malloc(sizeof(int));
}

void cbDestroy(CircularBuffer *cb)
{
    free(cb->data);
    free(cb); 
}

int cbIsEmpty(CircularBuffer *cb)
{
    if(cb->count==0) return(1);
    else return(0);
}

void cbWrite(CircularBuffer *cb, int val)
{
    if(cb->count>=cb->size)
    {
        printf("The queue is full\nYou cannot add items\n");
    }
    else
    {
        cb->count ++;
        cb->data[cb->last] = val;
        cb->last = ( cb->last + 1 ) % cb->size;
    }
}

int cbRead(CircularBuffer *cb)
{
    if(cbIsEmpty(cb))
    {
        printf("The queue is empty\n");
        return(-1);
    }
    else
    {
        int val = cb->data[cb->first];
        cb->first = ( cb->first + 1 ) % cb->size;
        cb->count --;
        return(val);
    }
}

void cbPrint(CircularBuffer *cb)
{
    int aux, aux1;
    aux  = cb->first;
    aux1 = cb->count;
    while(aux1>0)
    {
        printf("Element #%d = %d\n", aux, cb->data[aux]);
        aux=(aux+1)%cb->size;
        aux1--;
    }
    return;
}

int cbLast(CircularBuffer *cb)
{
    return cb->last -1 % cb->size;
}

int cbFirst(CircularBuffer *cb)
{
    return cb->first % cb->size; 
}

/* BUFFER TEST CODE
int main(int argc, char const *argv[])
{
	int i;
	int size = 5;
	CircularBuffer cb;

	cbInit(&cb, size);

	for (i=0; i<size; i++){

		cbWrite(&cb, i);
	}

	//cbPrint(&cb);
	printf("Popping index %d", cbFirst(&cb));
	printf(" = %d\n", cbRead(&cb));
	printf("Popping index %d", cbFirst(&cb));
	printf(" = %d\n", cbRead(&cb));

	printf("next write to index %d\n", cbLast(&cb));
	cbWrite(&cb, 5);
	printf("next write to index %d\n", cbLast(&cb));
	cbWrite(&cb, 6);
	printf("next write to index %d\n", cbLast(&cb));
	cbWrite(&cb, 7);

	printf("Popping index %d", cbFirst(&cb));
	printf(" = %d\n", cbRead(&cb));
	printf("Popping index %d", cbFirst(&cb));
	printf(" = %d\n", cbRead(&cb));
	//printf("Popping index %d = %d\n", cbFirst(&cb), cbRead(&cb));
	cbPrint(&cb);
	return 0;
}*/