#include <assert.h>
#include <stdio.h>
#include "binary_tree.h"
#include "util.h"

static int get_depth_at_index_static(struct node *, int, int *, int);
static int get_max_tree_depth_static(struct node *root, int curr_depth, int max_tree_depth);
static struct node *get_node_at_index_static(struct node **root, int goal_i, int *curr_i, struct node **goal);
static struct node *append_node_static(struct node **node, char value, bool side);
static int get_number_of_nodes_static(struct node **root, int cnt);
static void print_tree_static(struct node *root, int space, int delta_space);

/**
* Return the number of nodes in the tree. A recursive depth-first 
* right-to-left search is done.
* This function should not be called by itself. Call it's wrapper function.
	**root: The root node.
	   cnt: Current delta_space. 
*/
static int get_number_of_nodes_static(struct node **root, int cnt) {

	// Increase the delta_space
	cnt++;

	// Iterate over the children
	for (int i = 0; i < get_num_children(*root); i++) {
		// Recursively delta_space the child nodes
		struct node *child = get_children(root)[i];
		cnt = get_number_of_nodes_static(&child, cnt);
	}

	return cnt;
}


/**
* Return the node in the tree at a given index. The index is
* according to a depth-first left-to-right ordering.
* This function should not be called by itself. Call it's wrapper function.
	 **root: The root node.
	 goal_i: The index to search for.
	*curr_i: The current_index in the tree. This is necessary to be a pointer
			 as it must maintain value throughout seperate recursion loops.
	 **goal: Pointer to the pointer of the node that will be returned.
			 Must be initialized to work properly.
*/
static struct node *get_node_at_index_static(struct node **root, int goal_i, int *curr_i, struct node **goal) {

	if (goal_i < 0) return NULL;
	if (goal_i == 0) return *root;

	// Recursively loop through the tree
	// until the index is reached.
	for (int i = 0; i < get_num_children(*root); i++) {
		struct node *child = malloc(sizeof(struct node));
		child = get_children(root)[i];

		(*curr_i)++;

		if (*curr_i == goal_i) *goal = child;
		else get_node_at_index_static(&child, goal_i, curr_i, goal);
	}

	return *goal;
}


/**
* A wrapper function to make calling get_node_at_index_static() simpler.
	**root: The root node.
	  goal: The index to search for.
*/
struct node *get_node_at_index(struct node **root, int goal) {
	int index = 0;
	int *ptr = &index;
	struct node **tmp = malloc(sizeof(struct node **));

	return get_node_at_index_static(root, goal, ptr, tmp);
}


/**
* Return the max depth of the tree. Recursively traverse the tree.
* This function should not be called by itself. Call it's wrapper function.
		     *root: The root node.
	    curr_depth: The current depth in the tree.
	max_tree_depth: The current max depth that the function has reached.
*/
static int get_max_tree_depth_static(struct node *root,
	int curr_depth, int max_tree_depth) {

	// Update the max depth if the current depth is greater
	if (max_tree_depth < curr_depth) max_tree_depth = curr_depth;

	// Traverse the children of the root node
	for (int i = 0; i < get_num_children(root); i++) {
		struct node *child = get_children(&root)[i];

		// Recursively get the depth of the child node
		max_tree_depth = get_max_tree_depth_static(child, curr_depth + 1, max_tree_depth);
	}

	assert(curr_depth <= max_tree_depth);

	return max_tree_depth;
}


/**
* Return the depth of a node based on the index.
* The index is based on depth-first left-to-right traversal.
* This function should not be called by itself. Call it's wrapper function.
	      *root: The root tree node.
	     goal_i: The index to search for.
	    *curr_i: The current_index in the tree. This is necessary to be a pointer
			     as it must maintain value throughout seperate recursion loops.
	 curr_depth: The current depth in the tree. 
*/
static int get_depth_at_index_static(struct node *root, int goal_i,
	int *curr_i, int curr_depth) {
	if (goal_i < 0) return -1;
	if (goal_i == 0) return 0;

	static int i_depth = -1;

	if (goal_i == *curr_i) {
		i_depth = curr_depth;
	}

	for (int i = 0; (i_depth == -1) && i < get_num_children(root); i++) {
		struct node *child = get_children(&root)[i];

		(*curr_i)++;

		i_depth = get_depth_at_index_static(child, goal_i, curr_i, curr_depth + 1);
	}

	return i_depth;
}


/**
* Append a symbol to the node.
* Return NULL if child is not NULL, otherwise return the new node.
	**node: The (hopefully) soon to be parent node to append to. 
	 value: The value of the node to append.
	  side: The side of the parent node to append to.	
				0 = left side
				1 = right side
			Use the macros LEFT_SIDE and RIGHT_SIDE defined in
			the header.
*/
static struct node *append_node_static(struct node **node, char value, bool side) {
	struct node *node_new = new_node(value, NULL, NULL);

	if (side == LEFT_SIDE) {
		
		if ((*node)->left) return NULL;
		(*node)->left = node_new;

	} else if (side == RIGHT_SIDE) {
		
		if ((*node)->right) return NULL;
		(*node)->right = node_new;
	
	}

	return new_node;
}


/**
* Return if one node is equivalent to another. This is done by
* comparing their values and the pointers to their children.
	*n1, *n2: The nodes to compare.
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
	 **root: The root node.
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
* A wrapper to make calling get_number_of_nodes() simpler.
	*root: The root node.
*/
int get_number_of_nodes(struct node *root) {
	return get_number_of_nodes_static(&root, 0);
}


/**
* A wrapper function to make calling get_max_tree_depth_static() simpler.
	 *root: The root node.
*/
int get_max_tree_depth(struct node *root) {
	return get_max_tree_depth_static(root, 0, 0);
}

/**
* A wrapper to make call to get_depth_at_index_static() simpler.
	 *node: The root node.
	goal_i: The index to search for.
*/
int get_depth_at_index(struct node *node, int goal_i) {
	int index = 0;
	int *ptr = &index;

	return get_depth_at_index_static(node, goal_i, ptr, 0);
}


/**
 * A wrapper function to make calling append_node_static() simpler.
	 *node: The (hopefully) soon to be parent node to append to.
	 value: The value of the node to append.
	  side: The side of the parent node to append to.
				0 = left side
				1 = right side
			Use the macros LEFT_SIDE and RIGHT_SIDE defined in
			the header.
 */
struct node *append_node(struct node *node, char value, bool side) {
	struct node **ptr = &node;
	return append_node_static(ptr, value, side);
}

/**
 * A function to return a new node.
	  v: The value of the new node.
	 *l: The left child node.
	 *r: The right child node.
 */
struct node *new_node(char v, struct node *l, struct node *r) {
	struct node *node = malloc(sizeof(struct node));
	
	node->value = v;
	node->left = l;
	node->right = r;

	return node;
}

/**
* Replace a subtree.
	**old_tree: The old subtree to replace.
	**new_tree: The new subtree to replace old_tree.
*/
void replace_subtree(struct node **old_tree, struct node **new_tree) {
	// Change the old subtrees pointer to the new one.
	*old_tree = *new_tree;
}


/**
* Return the number of children of the node. The children
* are the root's immediate descendents.
	 *root: The root node.
*/
int get_num_children(struct node *root) {
	return (root->left ? 1 : 0) + (root->right ? 1 : 0);
}


/**
* Print the given node.
		*node: The node to print.
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
		      *root: The root node of the tree.
			  space: The amount of space between each tree.
		delta_space: The change in space as depth increases.
 */
static void print_tree_static(struct node *root, int space, int delta_space) {
	if (!root) return;

	space += delta_space;

	print_tree_static(root->right, space, delta_space);

	printf("\n");
	for (int i = delta_space; i < space; i++) {
		printf(" ");
	}
	printf("%c\n", root->value);

	print_tree_static(root->left, space, delta_space);
}


/**
 * Function wrapper to make caling print_tree_static simpler.
			  *root: The root node of the tree.
		delta_space: The change in space as depth increases.
 */
void print_tree(struct node *root, int delta_space) {
	print_tree_static(root, 0, delta_space);
}

