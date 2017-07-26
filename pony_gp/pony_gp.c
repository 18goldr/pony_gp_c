#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "structs.h"

#define MAX_DEPTH 7
#define INDEX 0

struct node *get_children(struct node *);
int get_num_children(struct node *);
int get_number_of_nodes(struct node *, int);
struct node *get_node_at_index(struct node *, int, int);
/*
 * Return the children of the node. 
 */
// return arrity
struct node *get_children(struct node *root) {
	short l = 0, r = 0;

	if (root->left)  l = 1;
	if (root->right) r = 1;

	if (!(r + l)) return (struct node *) malloc(0);

	struct node *children = malloc(sizeof(struct node) * (r+l));

	int i = 0;
	
	if (l) children[i++] = *root->left;
	if (r) children[i] = *root->right;

	return children;
}


int get_num_children(struct node *root) {
	return (root->left ? 1 : 0) + (root->right ? 1 : 0);
}


/*
 * Return the number of nodes in the tree. 
 */
// MUST PASS IN 0 for cnt;
int get_number_of_nodes(struct node *root, int cnt) {
	
	cnt++;
	int n = get_num_children(root);

	for (int i = 0; i < n; i++) {
		cnt = get_number_of_nodes(&get_children(root)[i], cnt);
	}

	return cnt;
}

struct node *get_node_at_index(struct node *root, int goal_i, int curr_i) {
	if (goal_i == 0) return root;

	static struct node *goal = NULL;

	for (int i = 0; i < get_num_children(root); i++) {
		struct node *child = &get_children(root)[i];

		curr_i++;

		if (curr_i == goal_i) goal = child;
		else get_node_at_index(child, goal_i, curr_i);
	}

	return goal;
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

	struct node *children = get_children(&tree[0]);

	printf("%c\n%c\n", children[0].value, children[1].value);
	int n = get_number_of_nodes(&tree[0], INDEX);

	printf("num nodes = %d\n", n);

	struct node *tmp = get_node_at_index(&tree[0], 6, 0);

	printf("%c\n", tmp->value);
}


//struct node *get_node_at_index(struct node *root, int idx) {
//	struct stack *unvisited_nodes = create_stack((int)pow(2, MAX_DEPTH));
//	push(unvisited_nodes, *root);
//
//	struct node *node = root;
//
//	int cnt = 0;
//
//	while (cnt <= idx && !is_empty(unvisited_nodes)) {
//		node = pop(unvisited_nodes);
//
//		struct node *children = get_children(node);
//
//		reverse(children);
//
//		for (int i = 0; i < get_num_children(node); i++) {
//			push(unvisited_nodes, children[i]);
//		}
//
//		cnt++;
//	}
//
//	return node;
//}
//
//void reverse(struct node *nodes) {
//	struct node *reverse = malloc(sizeof(nodes));
//	for (int i = get_number_of_nodes(nodes, 0) - 1; i >= 0; i--) {
//		*reverse = nodes[i];
//		reverse++;
//	}
//
//	nodes = reverse;
//}