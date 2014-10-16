/**************************************************
FILE: PrinterSpooler.c
DESCRIPTION: simulates a printer spooler using 
	pthreads and a shared circular buffer
AUTHOR: Jordan Quan
LAST REVISED: 2014/10/15
**************************************************/

// includes
#include "CircularBuffer.c"
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <pthread.h> 
#include <time.h>

// global variables
int clients, printers, bufferSize;
sem_t lock; // used to enforce mutual exclusion access of buffer
sem_t full; // counts full slots
sem_t empty; // counts empty slots

/*struct v {
	int name;
	CircularBuffer buffer;
};*/

/* reminder of cb methods
init, write, print, destroy, isempty, read, first, last */

void PrinterThread(CircularBuffer *cb, int aName)
{
	int name = aName;
    int index = 0;
    int pages;

    sem_wait(&full);
    sem_wait(&lock);
    //start critical
    index = cbFirst(cb);
    pages = cbRead(cb);
    //end critical
    sem_post(&lock);
    sem_post(&empty);

    printf("Printer %d starts printing %d pages from Buffer[%d]\n", name, pages, index);
    sleep(pages);
    printf("Printer %d finished printing %d pages from Buffer[%d]\n", name, pages, index);
}

void ClientThread(CircularBuffer *cb, int aName)
{
	/*struct v *data;
	data = (struct v *) arg;
    int name = data->name;
    CircularBuffer cb = data->buffer;*/

	int name = aName;
    srand(time(NULL));
    int pages = rand() % 10 + 1;
    int index = 0;

    sem_wait(&empty);
    sem_wait(&lock);
    //start critical
    cbWrite(cb, pages);
    index = cbLast(cb);
    //end critical
    sem_post(&lock);
    sem_post(&full);

    printf("Client %d has %d pages to print, puts request in Buffer[%d]\n", name, pages, index);

}

int main(int argc, char const *argv[])
{
    if (argc!=4)
    {
        printf("Please input # of [c]lients, [p]rinters, [b]ufferSize\n");
        exit(EXIT_FAILURE);
    }
    clients = atoi (argv[1]);
    printers = atoi (argv[2]);
    bufferSize = atoi (argv[3]);
    printf("clients %d, printers %d, bufferSize %d\n", clients, printers, bufferSize);

    //pthread_t clientThreads[clients];
    //pthread_t printerThreads[printers];

    sem_init(&lock, 0, 1);
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, bufferSize);

    CircularBuffer cb;
    cbInit(&cb, bufferSize);
    int delay = 1;
    int i,s;

    /*struct v *data = (struct v *) malloc(sizeof(struct v));
    data->name = 1;
    data->buffer = cb;

    pthread_create(&clientThreads[0], NULL, ClientThread, data);
    pthread_join(clientThreads[0], NULL);
    sleep(1);
    pthread_create(&clientThreads[1], NULL, ClientThread, data);
    pthread_join(clientThreads[1], NULL);*/

    for (i=0; i<clients; i++)
    {
    	ClientThread(&cb, i);
    	sleep(delay);
    	PrinterThread(&cb, i);
    } 

    return 0;
}