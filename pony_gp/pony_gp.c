#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "binary_tree.h"
#include "util.h"
#include "hashmap.h"

void grow(struct node *node, int curr_depth, int max_depth, bool full, struct symbols *symbols);
char get_random_symbol(int, int, struct symbols *, bool);
char *get_terminals(void); 
char *get_functions(void);
struct hashmap *get_arities(void);

/**
 * A struct to contain the different functions, terminals, and arities.
	 *terminals: An array of constant numbers that can be used in the program.
	 *functions: An array of functions that can be used in the program.
	   *arities: A hashmap containing the arities of different symbols
				to be used in the program.
 */
struct symbols {
	char *terminals;
	char *functions;
	struct hashmap *arities;
} symbols;


/*
 * Recursively grow a node to max depth in a pre-order, ie. depth-first
 * left-to-right traversal.
		      *node: The root node of the subtree.
	     curr_depth: The current depth in the tree. Must be passed in as 0 to work as
				     intended.
	 max_tree_depth: The current max depth that the function has reached.
	                 Must be passed in initially as 0 to work as intended.
	    should_fill: Grows the tree to max depth when true.
	       *symbols: The container for the functions, terminals and arities
	                 lists.
 */
void grow(struct node *node, int curr_depth,
		  int max_depth, bool must_fill, struct symbols *symbols) {
	char node_symbol = node->value;
	char symbol;

	if (curr_depth > max_depth) return;

	// grow is called recursively in the loop. The loop iterates
	// "arity" number of times, given by the node symbol.
	for (int side = 0; side < hashmap_get(symbols->arities, node_symbol); side++) {
		
		symbol = get_random_symbol(curr_depth, max_depth, symbols, must_fill);

		// new_node is not set with append_node() because it returns a copy of
		// the pointer to the new node, not the new node itself.
		append_node(node, symbol, side);
		struct node *new_node = (!side ? node->left : node->right);

		// call grow with the child node as the current node.
		grow(new_node, curr_depth+1, max_depth, must_fill, symbols);

	}

	assert(curr_depth <= max_depth);
}


/**
 * Return a randomly chosen symbol. The depth determines if a terminal
 * must be chosen. If `full` is specified a function will be chosen
 * until the max depth is reached. The symbol is picked with a uniform
 * probability.
	   curr_depth: The current depth. Must be passed in as 0 to perform
				   as intended.
	    max_depth: The max depth that the tree is allowed to reach.
	     *symbols: The container for the functions, terminals and arities
		     	   lists. 
	         full: True if function symbols should be used until max depth
				   is reached.
 */
char get_random_symbol(int curr_depth, int max_depth, struct symbols *symbols, bool full) {
	char symbol;
	int n;

	// Pick a terminal if max depth has been reached
	if (curr_depth >= max_depth - 1) {
		n = rand_index(get_char_arr_length(symbols->terminals)-1);
		symbol = *(symbols->terminals += n);
	} else {
		// 50% chance it will be a terminal if max depth has
		// not been reached
		if (!full && get_rand(0, 1)) {
			n = rand_index(get_char_arr_length(symbols->terminals)-1);
			symbol = *(symbols->terminals += n);
		} else {
			// Pick a random function
			n = rand_index(get_char_arr_length(symbols->functions)-1);
			symbol = *(symbols->functions += n);
		}
	}

	return symbol;
}
 

/**
 * Temporary function to get terminals. Will modify to read from
 * a config document.
 */
char *get_terminals() {
	char *terminals = malloc(3);
	terminals[0] = '0';
	terminals[1] = '1';
	terminals[2] = '\0';

	return terminals;
}


/**
* Temporary function to get functions. Will modify to read from
* a config document.
*/
char *get_functions() {
	char *functions = malloc(5);
	functions[0] = '+';
	functions[1] = '*';
	functions[2] = '/';
	functions[3] = '-';
	functions[4] = '\0';

	return functions;
}


/**
 * Temporary function to get functions. Will modify to read from
 * a config document.
 */
struct hashmap *get_arities() {
	struct hashmap *h = malloc(sizeof(struct hashmap));
	hashmap_init(h);

	hashmap_put(h, '+', 2);
	hashmap_put(h, '*', 2);
	hashmap_put(h, '/', 2);
	hashmap_put(h, '-', 2);
	hashmap_put(h, '0', 0);
	hashmap_put(h, '1', 0);

	return h;
}


main() {
	symbols.functions = get_functions();
	symbols.terminals = get_terminals();
	symbols.arities = get_arities();

	struct node *root = new_node('+', NULL, NULL);
	root->left = new_node('*', NULL, NULL);
	root->right = new_node('2', NULL, NULL);
	root->left->left = new_node('6', NULL, NULL);
	root->left->right = new_node('*', NULL, NULL);
	root->left->right->left = new_node('5', NULL, NULL);
	root->left->right->right = new_node('7', NULL, NULL);

	struct node *test = new_node('+', NULL, NULL);

	grow(test, 0, 5, false, &symbols);
	int n = 0;
	printf("%c", test->right->left->value);
}
