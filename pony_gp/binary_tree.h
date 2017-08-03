#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#define LEFT_SIDE 0
#define RIGHT_SIDE 1

void print_node(struct node *node);
struct node *append_node(struct node *, char, bool);
struct node **get_children(struct node **);
int get_depth_at_index(struct node *, int);
int get_max_tree_depth(struct node *root);
struct node *get_node_at_index(struct node **root, int goal);
int get_num_children(struct node *root);
int get_number_of_nodes(struct node *root);
bool matches(struct node *n1, struct node *n2);
void replace_subtree(struct node **old_tree, struct node **new_tree);
struct node *new_node(char v, struct node *l, struct node *r);
void print_tree(struct node *root, int delta_space);

/**
 * A structure to represent a binary tree node
	  value: The value of the node. Can be any mathematical terminal or
			 function with an arity of 2 or less.
	  *left: The child of this node in its left branch. Can be any valid
			 pointer value to a struct node (such as NULL).
	 *right: The child of this node in its left branch. Can be any valid
			 pointer value to a struct node (such as NULL).
 */
struct node {
	char value; 
	struct node *left, *right;
};
