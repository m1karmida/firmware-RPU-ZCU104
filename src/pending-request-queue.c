#include "pending-request-queue.h"
#include <stdio.h>
#include <stdlib.h>
 
/*LIST FUNCTIONS*/

queue_PR * init_queue_PR(){
	queue_PR *coda = (queue_PR *) malloc(sizeof(queue_PR));
	coda->head = NULL;
	coda->tail = NULL;
	coda->size = 0;
	return coda;
}


bool queuePRIsEmpty(queue_PR* coda){
	return (coda->size == 0);
}

void push_queuePR(queue_PR *coda, uint32_t code){
    if(coda == NULL){
        printf("Attempting to read a deallocating list\n");
        return;
    }

    if(coda->size == 0)
    {
        coda->head = (node_PR_t *) malloc(sizeof(node_PR_t));
        coda->head->code =  code;
        coda->head->next = NULL;
        coda->tail = coda->head;

        coda->size = 1;
    }
    else{
        coda->tail->next = (node_PR_t *) malloc(sizeof(node_PR_t));
        coda->tail = coda->tail->next;
        coda->tail->code = code;
        coda->tail->next = NULL;

        coda->size++;
    }
}

int pop_queuePR(queue_PR *coda) {
    if(coda == NULL){
        printf("Attempting to read a deallocating list\n");
        return -1;
    }

    int retval = -1;
    node_PR_t * next_node = NULL;

    if (coda->head == NULL) {
        return -1;
    }

    next_node = coda->head->next;
    retval = coda->head->code;
    free(coda->head);
    coda->head = next_node;

    coda->size--;

    return retval;
}

void print_queuePR(queue_PR *coda) {
    if(coda == NULL){
        printf("Attempting to read a deallocating list\n");
        return;
    }

    node_PR_t * current = coda->head;

    while (current != NULL) {
        printf("Code:    %d\n", current->code);
        current = current->next;
    }

    printf("Queue size = %d\n\n", coda->size);
}

int remove_by_value_queuePR(queue_PR *coda, uint32_t code){
    if(coda == NULL){
        printf("Attempting to read a deallocating queue\n");
        return -1;
    }
    node_PR_t *previous, *current;

    if (coda->head == NULL) {
        return -1;
    }

    if (coda->head->code == code) {
        return pop_queue(coda);
    }

    previous = coda->head;
    current = coda->head->next;
    while (current) {
        if (current->code == code) {
            previous->next = current->next;
            free(current);
            coda->size--;
            return code;
        }

        previous = current;
        current  = current->next;
    }
    return -1;
}

void delete_queuePR(queue_PR **coda){
    if(coda == NULL){
        printf("Attempting to read a deallocating list\n");
        return;
    }
    while ((*coda)->size > 0) {
        pop_queue(*coda);
    }

    (*coda)->head = NULL;
    (*coda)->tail = NULL;
    (*coda)->size = 0;

    free(*coda);

    *coda = NULL;
}

