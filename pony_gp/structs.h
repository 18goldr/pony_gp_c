#define LEFT_SIDE 0
#define RIGHT_SIDE 1

// A structure to represent a binary tree node
struct node {
	char value;
	struct node *left, *right;
};

bool matches(struct node *n1, struct node *n2) {
	return (n1->value == n2->value &&
		    n1->left  == n2->left  &&
		    n1->right == n2->right
	);
}


