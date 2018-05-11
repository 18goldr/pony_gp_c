#ifndef PONY_GP_QUEUE_H
#define PONY_GP_QUEUE_H

#include <stdbool.h>
#include <stdlib.h>
#include "../include/memmngr.h"
/**
 * An implementation of the queue data structure.
 * @field front The front node of the queue.
 * @field rear The rear node of the queue.
 * @field size The size of the queue.
 */
struct queue {
    struct q_node *front, *rear;
    int size;
};

/**
 * A wrapper to hold a binary tree node in a queue.
 * @field value The binary tree node.
 * @field next The node directly preceding the current node.
 */
struct q_node {
    struct node *value;
    struct q_node *next;
};

struct queue *create_queue(void);
struct q_node *new_q_node(struct node *node);
bool is_empty(struct queue *q);
void enqueue(struct queue *q, struct node *node);
struct q_node *dequeue(struct queue *q);
void free_queue(struct queue *q);
void free_q_node(struct q_node *qn);

#endif //PONY_GP_QUEUE_H
