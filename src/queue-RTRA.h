/*
 * queue-RTRA.h
 *
 *  Created on: 8 gen 2022
 *      Author: michele
 */

#ifndef INC_QUEUE_RTRA_H_
#define INC_QUEUE_RTRA_H_


#include <stdbool.h>


typedef struct Pending_request{
	TickType_t RA;
	struct Pending_request *next;
}node;

typedef struct queue{
    node * head;						//Head of the List
    node * tail;						//Tail of the List
    int size;
} RTRAqueue;

/*LIST INTERFACES*/

RTRAqueue * init_queue();
bool queueIsEmpty(RTRAqueue *coda);
void push_queue(RTRAqueue *coda, TickType_t RA);

TickType_t pop_queue(RTRAqueue *coda);

void delete_queue(RTRAqueue **coda);




#endif /* INC_QUEUE_RTRA_H_ */
