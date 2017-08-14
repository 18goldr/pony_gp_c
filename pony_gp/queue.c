#include <stdlib.h>
#include "queue.h"

bool is_empty(struct queue *q) {
	return (!q->front);
}

struct q_node *new_q_node(struct node *node) {
	struct q_node *tmp = malloc(sizeof(struct q_node));
	tmp->value = node;
	tmp->next = NULL;

	return tmp;
}

struct queue *create_queue() {
	struct queue *q = malloc(sizeof(struct queue));
	q->front = q->rear = NULL;
	q->size = 0;
	return q;
}

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

int queue_size(struct queue *q) {
	return q->size;
}