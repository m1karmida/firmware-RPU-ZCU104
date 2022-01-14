//QUEUE: it contains all the pending requested task on RPU
#ifndef _PENDING-REQUEST-QUEUE.H
#define _PENDING-REQUEST-QUEUE.H

#include <stdbool.h>
#include <stdint.h>


typedef struct pendingRequest{
	uint32_t code;						//code to request the task (identify the requested task)
    struct pendingRequest *next;
}node_PR_t;

typedef struct Queue{
    node_PR_t * head;						//Head of the List
    node_PR_t * tail;						//Tail of the List
    uint16_t size;
} queue_PR;

/*LIST INTERFACES*/

queue_PR * init_queue_PR();
bool queuePRIsEmpty(queue_PR* coda);
void push_queuePR(queue_PR *coda, uint32_t code);
int pop_queuePR(queue_PR *coda);
void print_queuePR(queue_PR *coda);
int remove_by_value_queuePR(queue_PR *coda, uint32_t code);
void delete_queuePR(queue_PR **coda);


#endif
