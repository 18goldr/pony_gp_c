#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "structs.h"

#define MAX_DEPTH 7

void test(void);
int append_node(struct node *, char, bool);
void replace_subtree(struct node *, struct node **);
struct node *get_children(struct node *);
int get_num_children(struct node *);
int get_number_of_nodes(struct node *, int);
struct node *get_node_at_index(struct node *, int, int *);
int get_max_tree_depth(struct node *, int, int);
int get_depth_at_index(struct node *, int, int *, int);


/*
 * Append a symbol to the node. Return success value.
 * Return 1 if child is not NULL (failure), otherwise return 0.
 */
int append_node(struct node *node, char value, bool side) {
	struct node *new_node = malloc(sizeof(struct node));

	new_node = &(struct node) { value, NULL, NULL };

	if (side == LEFT_SIDE) {
		if (node->left) return(EXIT_FAILURE);
		node->left = new_node;

	} else if (side == RIGHT_SIDE) {
		if (node->right) return(EXIT_FAILURE);
		node->right = new_node;
	}

	return(EXIT_SUCCESS);
}


/*
* Replace a subtree.
*/
void replace_subtree(struct node *new_tree, struct node **old_tree) {
	// Change the old subtrees pointer to the new one.
	*old_tree = new_tree;
}


/*
 * Return the children of the node. The children are
 * the root's immediate descendents.
 */
struct node *get_children(struct node *root) {
	short l = 0, r = 0;

	// Check that the root has a left and right descendent.
	if (root->left)  l = 1;
	if (root->right) r = 1;

	// If neither, it has no children.
	if (!(r + l)) return (struct node *) malloc(0);

	// Determine array capacity based on the number of children;
	struct node *children = malloc(sizeof(struct node) * (r+l));

	int i = 0;
	
	if (l) children[i++] = *root->left;
	if (r) children[i] = *root->right;

	return children;
}


/*
 * Return the number of children of the node. The children
 * are the root's immediate descendents.
 */
int get_num_children(struct node *root) {
	return (root->left ? 1 : 0) + (root->right ? 1 : 0);
}


/*
 * Return the number of nodes in the tree. 
 * Must pass in 0 for cnt to work properly.
 */
int get_number_of_nodes(struct node *root, int cnt) {
	
	// Increase the count
	cnt++;

	// Iterate over the children
	for (int i = 0; i < get_num_children(root); i++) {
		// Recursively count the child nodes
		cnt = get_number_of_nodes(&get_children(root)[i], cnt);
	}

	return cnt;
}


/*
 * Return the node in the tree at a given index. The index is
 * according to a depth-first left-to-right ordering.
 */
struct node *get_node_at_index(struct node *root, int goal_i, int *curr_i) {

	if (goal_i < 0) return NULL;
	if (goal_i == 0) return root;

	static struct node *goal = NULL;

	// Recursively loop through the tree
	// until the index is reached.
	for (int i = 0; !goal && i < get_num_children(root); i++) {
		struct node *child = &get_children(root)[i];

		(*curr_i)++;

		if (*curr_i == goal_i) goal = child;
		else get_node_at_index(child, goal_i, curr_i);
	}

	return goal;
}


/*
 * Return the max depth of the tree. Recursively traverse the tree.
 */
int get_max_tree_depth (struct node *root, 
						int curr_depth, int max_tree_depth) {

	// Update the max depth if the current depth is greater
	if (max_tree_depth < curr_depth) max_tree_depth = curr_depth;

	// Traverse the children of the root node
	for (int i = 0; i < get_num_children(root); i++) {
		struct node *child = &get_children(root)[i];

		// Recursively get the depth of the child node
		max_tree_depth = get_max_tree_depth(child, curr_depth+1, max_tree_depth);
	}

	return max_tree_depth;
}

/*
 * Return the depth of a node based on the index. 
 * The index is based on depth-first left-to-right traversal.
 */
int get_depth_at_index(struct node *node, int goal_i, int *curr_i, int curr_depth) {
	if (goal_i < 0) return -1;
	if (goal_i == 0) return 0;

	static int i_depth = -1;

	if (goal_i == *curr_i) {
		i_depth = curr_depth;
	}

	for (int i = 0; (i_depth == -1) && i < get_num_children(node); i++) {
		struct node *child = &get_children(node)[i];

		(*curr_i)++;

		i_depth = get_depth_at_index(child, goal_i, curr_i, curr_depth+1);
	}

	return i_depth;
}


main() {
	struct node tree[] = {
		{'+', &tree[1], &tree[6]}, // tree[0]
		{'*', &tree[2], &tree[3]}, // tree[1]
		{'6', NULL,     NULL    }, // tree[2]
		{'*', &tree[4], &tree[5]}, // tree[3]
		{'5', NULL,     NULL    }, // tree[4]
		{'7', NULL,     NULL    }, // tree[5]
		{'2', NULL,     NULL    }  // tree[6]
	};

}
