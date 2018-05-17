
#include "../include/queue.h"

/**
 * Allocate the memory for a queue. Initialize fields.
 * @return The empty queue.
 */
struct queue *create_queue() {
    struct queue *q = allocate_m(sizeof(struct queue));
    q->front = q->rear = NULL;
    q->size = 0;

    return q;
}

/**
 * Allocate the memory for a queue node. Set the value of the node.
 * @param node The value of the queue node.
 * @return The queue node.
 */
struct q_node *new_q_node(struct node *node) {
    struct q_node *qn = allocate_m(sizeof(struct q_node));
    qn->value = node;
    qn->next = NULL;

    return qn;
}

/**
 * Check if the queue is empty.
 * @param q The queue to check
 * @return Whether the queue is empty or not.
 */
bool is_empty(struct queue *q) {
    return (!q->front);
}

/**
 * Append a node to the back of the queue.
 * @param q The queue to append to.
 * @param node The binary tree node to add to the queue.
 */
void enqueue(struct queue *q, struct node *node) {
    struct q_node *n = new_q_node(node);

    q->size++;

    if (!q->rear) {
        q->front = q->rear = n;
        return;
    }

    q->rear->next = n;
    q->rear = n;
}

/**
 * Remove the front node of the queue.
 * @param q The queue to modify.
 * @return The front node that was removed.
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
 * Free memory allocated for a queue.
 * Frees the queue's nodes as well.
 * @param q The queue to free.
 */
void free_queue(struct queue *q) {
    while (q->front) {
        struct q_node *front = q->front;
        q->front = q->front->next;
        free_q_node(front);
    }

    free_pointer(q);
}

/**
 * Free memory allocated for a queue node.
 * @param qn The queue node to free.
 */
void free_q_node(struct q_node *qn) {
    qn->next = NULL;

    free_pointer(qn);
}