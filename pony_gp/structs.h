// A structure to represent a binary tree node
struct node {
	char value;
	struct node *left, *right;
};

// A structure to represent a stack
struct stack {
	int top;
	unsigned capacity;
	struct node *array;
};

// function to create a stack of given capacity. It initializes size of
// stack as 0
struct stack* create_stack(unsigned capacity) {
	struct stack* stack = (struct stack*) malloc(sizeof(struct stack));
	stack->capacity = capacity;
	stack->top = -1;
	stack->array = malloc(stack->capacity * sizeof(struct node));
	return stack;
}

// stack is full when top is equal to the last index
int is_full(struct stack* stack) {
	return stack->top == stack->capacity - 1;
}

// stack is empty when top is equal to -1
int is_empty(struct stack* stack) {
	return stack->top == -1;
}

// Function to add an item to stack.  It increases top by 1
void push(struct stack* stack, struct node tree_node) {
	if (is_full(stack)) return;

	stack->array[++stack->top] = tree_node;

	printf("{value: %c, left: %p, right: %p} pushed to stack\n", 
			tree_node.value, tree_node.left, tree_node.right);
}

// Function to remove an item from stack.  It decreases top by 1
struct node *pop(struct stack* stack) {
	if (!is_empty(stack)) 
		return &(stack->array[stack->top--]);

	return NULL;
}

