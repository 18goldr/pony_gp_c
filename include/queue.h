#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

/**
* An implementation of the queue data structure.
*	front: The front of the queue.
*	 rear: The rear of the queue.
*	 size: The size of the queue.
*/
struct queue {
	struct q_node *front, *rear;
	int size;
};

/**
* A binary tree member of a queue.
*	value: The value of the member.
*	 next: The next member in the queue.
*/
struct q_node {
	struct node *value;
	struct q_node *next;
};

bool is_empty(struct queue *queue);
struct q_node *new_q_node(struct node *node);
struct queue *create_queue(void);
void enqueue(struct queue *queue, struct node *node);
struct q_node *dequeue(struct queue *q);
int queue_size(struct queue *q);
void deinit_queue(struct queue *q);

#endif