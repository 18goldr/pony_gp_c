#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

struct q_node {
	struct node *value;
	struct q_node *next;
};

struct queue {
	struct q_node *front, *rear;
	int size;
};

bool is_empty(struct queue *queue);
struct q_node *new_q_node(struct node *node);
struct queue *create_queue(void);
void enqueue(struct queue *queue, struct node *node);
struct q_node *dequeue(struct queue *q);
int queue_size(struct queue *q);
void deinit_queue(struct queue *q);

#endif