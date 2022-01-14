#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue-RTRA.h"


//TODO: implementare coda fifo

/*QUEUE FUNCTIONS*/


RTRAqueue * init_queue(){
	RTRAqueue *queue = (RTRAqueue *) malloc(sizeof(RTRAqueue));
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;


	return queue;
}

bool queueIsEmpty(RTRAqueue* coda){
	return (coda->size == 0);
}

void push_queue(RTRAqueue *coda, TickType_t RA){
    if(coda == NULL){
        printf("Attempting to read a deallocating queue\n");
        return;
    }

    if(coda->size == 0)
    {
        coda->head = (node *) malloc(sizeof(node));
        coda->head->RA =  RA;
        coda->head->next = NULL;
        coda->tail = coda->head;

        coda->size = 1;
    }
    else{
        coda->tail->next = (node *) malloc(sizeof(node));
        coda->tail = coda->tail->next;
        coda->tail->RA = RA;

        coda->size++;
    }
}

TickType_t pop_queue(RTRAqueue *coda) {
    if(coda == NULL){
        printf("Attempting to read a deallocating queue\n");
        return NULL;
    }

    TickType_t retval = coda->head->RA;
    node * next_node = NULL;

    if (coda->head == NULL) {
        return NULL;
    }

    next_node = coda->head->next;
    free(coda->head);
    coda->head = next_node;

    coda->size--;

    return retval;
}


void delete_queue(RTRAqueue **coda) {
    if(coda == NULL){
        printf("Attempting to read a deallocating queue\n");
        return;
    }
    while ((*coda)->size > 0) {
        pop_queue(*coda);
        //print_list(*Lista);
    }
    (*coda)->head = NULL;
    (*coda)->tail = NULL;
    (*coda)->size = 0;

    free(*coda);

    *coda = NULL;
}

