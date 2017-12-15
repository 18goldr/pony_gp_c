#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include "queue.h"
#include "binary_tree.h"

static int get_depth_at_index_util(struct node *root, int goal_i, int *curr_i, int curr_depth, int *i_depth);
static struct node *get_node_at_index_util(struct node **root, int goal_i, int *curr_i, struct node **goal);
static struct node *append_node_util(struct node **node, char value, bool side);
static void print_tree_util(struct node *root, int space, int delta_space);
static char *to_string_util(struct node *root, char *str, int *cur_pos);

/**
* Function to count the number of nodes in a tree with recursion and
* without a stack using the Morris Tree Taversal algorithm.
*	 root: The root of the tree.
*/
int get_number_of_nodes(struct node *root) {
	if (!root) return 0;

	int count = 0;
	struct node *pre, *current = root;

	while (current) {
		if (!current->left) {
			count++;
			current = current->right;
		}
		else {
			pre = current->left;

			while (pre->right && pre->right != current) {
				pre = pre->right;
			}

			if (!pre->right) {
				pre->right = current;
				current = current->left;
				
			}
			else {
				pre->right = NULL;
				count++;
				current = current->right;
			}
		}
	}

	return count;
}


/**
* Return the node in the tree at a given index. The index is
* according to a depth-first left-to-right ordering.
* This function should not be called by itself. Call it's wrapper function.
*	   root: The root node.
*	 goal_i: The index to search for.
*	 curr_i: The current_index in the tree. This is necessary to be a pointer
*			 as it must maintain value throughout seperate recursion loops.
*	   goal: Pointer to the pointer of the node that will be returned.
*			 Must be initialized to work properly.
*/
static struct node *get_node_at_index_util(struct node **root, int goal_i, int *curr_i, struct node **goal) {

	if (goal_i < 0) return NULL;
	if (goal_i == 0) return *root;

	// Recursively loop through the tree
	// until the index is reached.
	for (int i = 0; i < get_num_children(*root); i++) {
		struct node *child = malloc(sizeof(struct node));
		child = get_children(root)[i];

		(*curr_i)++;

		if (*curr_i == goal_i) *goal = child;
		else get_node_at_index_util(&child, goal_i, curr_i, goal);
	}

	return *goal;
}


/**
* A wrapper function to make calling get_node_at_index_util() simpler.
*	root: The root node.
*	goal: The index to search for.
*/
struct node *get_node_at_index(struct node **root, int goal) {
	int index = 0;
	int *ptr = &index;
	struct node **tmp = malloc(sizeof(struct node **));

	return get_node_at_index_util(root, goal, ptr, tmp);
}

/**
* Return the max depth of the tree. Iteratively traverse the tree with
* a queue.
*	 root: The root of the tree.
*/
int get_max_tree_depth(struct node *root) {
	if (!root) return 0;

	struct queue *q = create_queue();

	enqueue(q, root);
	int height = 0;
	int node_count;

	while ((node_count = queue_size(q))) {
		height++;

		while (node_count > 0) {
			struct node *node = q->front->value;
			free(dequeue(q));

			if (node->left) {
				enqueue(q, node->left);
			}

			if (node->right) {
				enqueue(q, node->right);
			}

			node_count--;
		}
	}

	free(q);

	 //Height is one more than it should be.
	return height-1;

}


/**
* Return the depth of a node based on the index.
* The index is based on depth-first left-to-right traversal.
* This function should not be called by itself. Call it's wrapper function.
*	       root: The root tree node.
*	     goal_i: The index to search for.
*	     curr_i: The current_index in the tree. This is necessary to be a pointer
*			     as it must maintain value throughout seperate recursion loops.
*	 curr_depth: The current depth in the tree. 
*/
static int get_depth_at_index_util(struct node *root, int goal_i,
	int *curr_i, int curr_depth, int *i_depth) {
	if (goal_i < 0) return -1;
	if (goal_i == 0) return 0;

	if (goal_i == *curr_i) {
		*i_depth = curr_depth;
	}

	for (int i = 0; (*i_depth == -1) && i < get_num_children(root); i++) {
		struct node *child = get_children(&root)[i];

		(*curr_i)++;

		*i_depth = get_depth_at_index_util(child, goal_i, curr_i, curr_depth + 1, i_depth);
	}

	return *i_depth;
}


/**
* A wrapper to make call to get_depth_at_index_util() simpler.
*	  node: The root node.
*	goal_i: The index to search for.
*/
int get_depth_at_index(struct node *node, int goal_i) {
	int index = 0;
	int i_depth = -1;

	return get_depth_at_index_util(node, goal_i, &index, 0, &i_depth);
}


/**
* Append a symbol to the node.
* Return NULL if child is not NULL, otherwise return the new node.
*	  node: The (hopefully) soon to be parent node to append to. 
*	 value: The value of the node to append.
*	  side: The side of the parent node to append to.	
*				0 = left side
*				1 = right side
*			Use the macros LEFT_SIDE and RIGHT_SIDE defined in
*			the header.
*/
static struct node *append_node_util(struct node **node, char value, bool side) {
	struct node *node_new = new_node(value);

	if (side == LEFT_SIDE) {
		
		if ((*node)->left) return NULL;
		(*node)->left = node_new;

	} else if (side == RIGHT_SIDE) {
		
		if ((*node)->right) return NULL;
		(*node)->right = node_new;
	
	}

	return node_new;
}


/**
* A wrapper function to make calling append_node_util() simpler.
*	  node: The (hopefully) soon to be parent node to append to.
*	 value: The value of the node to append.
*	  side: The side of the parent node to append to.
*				0 = left side
*				1 = right side
*			Use the macros LEFT_SIDE and RIGHT_SIDE defined in
*			the header.
*/
struct node *append_node(struct node *node, char value, bool side) {
	struct node *node_new = new_node(value);

	if (side == LEFT_SIDE) {

		if (node->left) return NULL;
		node->left = node_new;

	}
	else if (side == RIGHT_SIDE) {

		if (node->right) return NULL;
		node->right = node_new;

	}

	return node_new;

}


/**
* Return if one node is equivalent to another. This is done by
* comparing their values and the pointers to their children.
*	 n1, n2: The nodes to compare.
*/
bool matches(struct node *n1, struct node *n2) {
	return (n1->value == n2->value &&
		n1->left == n2->left  &&
		n1->right == n2->right
	);
}


/**
* Return the children of the node. The children are
* the root's immediate descendents.
*	 root: The root node.
*/
struct node **get_children(struct node **root) {
	short l = 0, r = 0;
	// Check that the root has a left and right descendent.
	if ((*root)->left)  l = 1;
	if ((*root)->right) r = 1;

	// If neither, it has no children.
	if (!(r + l)) return (struct node **) malloc(0);

	// Determine array capacity based on the number of children;
	struct node **children = malloc(sizeof(struct node *) * (r + l));

	int i = 0;

	if (l) children[i++] = (*root)->left;
	if (r) children[i] = (*root)->right;

	return children;
}


/**
* Return a node with no children.
*	 v: The value of the node
*/
struct node *new_node(char v) {
	struct node *node = malloc(sizeof(struct node));

	node->value = v;
	node->left = NULL;
	node->right = NULL;

	return node;
}

void free_node(struct node *node) {
	if (node->left) free_node(node->left);
	if (node->right) free_node(node->right);

	free(node);
}

/**
* Replace a subtree.
*	 old_tree: The old subtree to replace.
*	 new_tree: The new subtree to replace old_tree.
*/
void replace_subtree(struct node **old_tree, struct node **new_tree) {
	// Change the old subtrees pointer to the new one.
	*old_tree = *new_tree;
}


/**
* Return the number of children of the node. The children
* are the root's immediate descendents.
*	 root: The root node.
*/
int get_num_children(struct node *root) {
	return (root->left ? 1 : 0) + (root->right ? 1 : 0);
}


/**
* Print the given node.
*	 node: The node to print.
*/
void print_node(struct node *node) {
	if (node) {
		printf("%c:", node->value);

		if (node->left) printf(" %c", node->left->value);
		if (node->right) printf(" %c", node->right->value);
	}

	printf("\n");
	
}


/**
* Print a tree. Left is the top, right is the bottom.
* Should not be called by itself. Call it's wrapper function.
*		    root: The root node of the tree.
*		   space: The amount of space between each tree.
*	 delta_space: The change in space as depth increases.
*/
static void print_tree_util(struct node *root, int space, int delta_space) {
	if (!root) return;

	space += delta_space;

	if (root->right)
		print_tree_util(root->right, space, delta_space);

	printf("\n");
	for (int i = delta_space; i < space; i++) {
		printf(" ");
	}
	printf("%c\n", root->value);

	if (root->left)
		print_tree_util(root->left, space, delta_space);
}


/**
* Function wrapper to make caling print_tree_util simpler.
*		    root: The root node of the tree.
*	 delta_space: The change in space as depth increases.
*/
void print_tree(struct node *root, int delta_space) {
	print_tree_util(root, 0, delta_space);
}

/**
* Prints the nodes in a tree in index order ie.
* top-bottom right-to-left traversal.
*	  root: The root of the tree to print.
*/
void print_nodes_index_order(struct node *root) {
	int num_nodes = get_number_of_nodes(root);

	for (int i = 0; i < num_nodes; i++) {
		printf("'%c'", get_node_at_index(&root, i)->value);

		if (i != num_nodes - 1) {
			printf(", ");
		}
		else {
			printf("\n");
		}
	}
}

/**
* Utility to get a string of a tree nodes in index order
* (ie. top-bottom right-to-left traversal).
*	    root: The root node of the tree.
*	     str: The str to modify, usually an empty string.
*	 cur_pos: The current position in the string.
*/
static char *to_string_util(struct node *root, char *str, int *cur_pos) {

	if (!root) return "";

	str[(*cur_pos)++] = root->value;

	if (root->left) to_string_util(root->left, str, cur_pos);

	if (root->right) to_string_util(root->right, str, cur_pos);

	return str;
}

/**
* Return a string with a trees node values in index order
* (ie. top-bottom right-to-left traversal).
*	 root: The root node of the tree
*/
char *tree_to_string(struct node *root) {
	int num_nodes = get_number_of_nodes(root);
	char *str = malloc(num_nodes + 1); // Leave space for null character
	int pos = 0;

	str = to_string_util(root, str, &pos);

	str[num_nodes] = '\0';

	return str;
}

/**
* Recursively create a deep copy of a tree.
*	 root: The root node of the tree.
*/
struct node *tree_deep_copy(struct node *root) {
	struct node *copy = NULL;

	if (root) {
		copy = new_node(root->value);
		copy->left = tree_deep_copy(root->left);
		copy->right = tree_deep_copy(root->right);
	}

	return copy;
}
