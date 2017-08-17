#include <stdlib.h>
#include "queue.h"

/**
* Return if the queue is empty.
*	q: The queue to check.
*/
bool is_empty(struct queue *q) {
	return (!q->front);
}

/**
* Create a new q_node.
*	 node: The value of the q_node.
*/
struct q_node *new_q_node(struct node *node) {
	struct q_node *tmp = malloc(sizeof(struct q_node));
	tmp->value = node;
	tmp->next = NULL;

	return tmp;
}

/**
* Create an empty queue.
*/
struct queue *create_queue() {
	struct queue *q = malloc(sizeof(struct queue));
	q->front = q->rear = NULL;
	q->size = 0;
	return q;
}

/**
* Add a node to the back of the queue.
*	    q: The queue to add to.
*	 node: The node to add.
*/
void enqueue(struct queue *q, struct node *node) {
	struct q_node *tmp = new_q_node(node);
	
	q->size++;

	if (!q->rear) {
		q->front = q->rear = tmp;
		return;
	}

	q->rear->next = tmp;
	q->rear = tmp;
}

/**
* Remove the head of the queue.
*	 q: The queue to modify.
*/
struct q_node *dequeue(struct queue *q) {
	if (!q->front) {
		return NULL;
	}

	struct q_node *tmp = q->front;
	q->front = q->front->next;

	if (!q->front) {
		q->rear = NULL;
	}

	q->size--;

	return tmp;
}

/**
* Return the queue size.
*	 q: The queue.
*/
int queue_size(struct queue *q) {
	return q->size;
}