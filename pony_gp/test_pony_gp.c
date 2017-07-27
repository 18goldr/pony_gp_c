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



void test() {
	struct node branch[] = {
		{ '+', &branch[1], &branch[6] }, // branch[0]
		{ '*', &branch[2], &branch[3] }, // branch[1]
		{ '6', NULL,     NULL }, // branch[2]
		{ '*', &branch[4], &branch[5] }, // branch[3]
		{ '5', NULL,     NULL }, // branch[4]
		{ '7', NULL,     NULL }, // branch[5]
		{ '2', NULL,     NULL }  // branch[6]
	};

	struct node *children = get_children(&branch[0]);

	printf("%c\n%c\n", children[0].value, children[1].value);
	int n = get_number_of_nodes(&branch[0], 0);

	printf("num nodes = %d\n", n);

	int index = 0;

	struct node *tmp = get_node_at_index(&branch[0], 3, &index);

	printf("%c\n", tmp->value);

	int max_depth = get_max_tree_depth(&branch[0], 0, 0);

	printf("%d\n", max_depth);

	int index2 = 0;

	printf("%d\n", get_depth_at_index(&branch[0], 4, &index2, 0));

	printf("%i\n", append_node(&branch[1], 'x', RIGHT_SIDE));

	printf("%c\n", *(&branch[1].right->value));

	struct node *new = &(struct node) { 'x', NULL, NULL };
	
	struct node *ptr = &branch[1];
	struct node *ptr1 = &ptr;

	replace_subtree(new, ptr1);

	printf("%c, %p, %p\n", ptr->value, ptr->left, ptr->right);
}