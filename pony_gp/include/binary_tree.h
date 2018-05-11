#ifndef PONY_GP_BINARY_TREE_H
#define PONY_GP_BINARY_TREE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/queue.h"
#include "../include/memmngr.h"
#include "../include/params.h"

#define LEFT_SIDE 0
#define RIGHT_SIDE 1

/**
 * A binary tree node.
 * @field value The terminal/function that the node holds.
 * @field left The child of the node contained in its left branch.
 * @field right The child of the node contained in its right branch.
 */
struct node {
    char value;
    struct node *left, *right;
};

struct node *new_node(char v);
void free_node(struct node *node);
void print_tree(struct node *root, int space, int delta_space);
int get_number_of_nodes(struct node *root);
struct node **get_children(struct node *root);
int get_num_children(struct node *root);
int get_depth_at_index_wrapper(struct node *root, int goal_i);
int get_depth_at_index(struct node *root, int goal_i, int *curr_i, int curr_depth, int *i_depth);
struct node *get_node_at_index_wrapper(struct node *root, int goal);
struct node *get_node_at_index(struct node *root, int goal_i, int *curr_i, struct node **goal);
struct node *append_node(struct node *tree, char value, bool side);
void print_nodes_index_order(struct node *root);
struct node *append_node(struct node *node, char value, bool side);
int get_max_tree_depth(struct node *root);
struct node *tree_deep_copy(struct node *node);

#endif //PONY_GP_BINARY_TREE_H
