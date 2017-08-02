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
struct hashmap *get_params(void);
void subtree_mutation(struct node **node, struct hashmap *params, struct symbols *symbols);

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


/**
 * An individual is a struct that contains:
 *		 "genome": A tree.
 *		"fitness": The fitness of the evaluated tree.
 */
struct individual {
	struct node *genome;
	double fitness;
};


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
	char node_arr[2];
	node_arr[0] = node_symbol;
	node_arr[1] = '\0';

	char symbol;

	if (curr_depth >= max_depth) return;

	// grow is called recursively in the loop. The loop iterates
	// "arity" number of times, given by the node symbol.
	for (int side = 0; side < hashmap_get(symbols->arities, node_arr); side++) {
		
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
 * Subtree mutation. Pick a node and grow it.
	   **root: The root node of the tree.
	  *params: A hashmap containing user defined parameters.
	 *symbols: The container for the functions, terminals and arities
	           lists.
 */
void subtree_mutation(struct node **root, struct hashmap *params, struct symbols *symbols) {

	// Check if mutation should be applied
	if (get_rand_probability() < hashmap_get(params, "mutation_probability")) {
		// Pick a node
		int end_node_i = get_number_of_nodes(*root) - 1;
		int node_i = get_randint(0, end_node_i);
		struct node *old_node = get_node_at_index(root, node_i);

		// Get new subtree
		int node_depth = get_depth_at_index(*root, node_i);
		
		char new_symbol = get_random_symbol(
				node_depth, (int)hashmap_get(params, "max_depth") - node_depth, symbols, false
		);

		struct node *new_subtree = new_node(new_symbol, NULL, NULL);

		// Grow the subtree
		grow(new_subtree, 0, (int)hashmap_get(params, "max_depth"), false, symbols);
		
		// Replace the old subtree with the new one
		*old_node = *new_subtree;
	}
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
	if (curr_depth > max_depth - 1) {
		n = rand_index(get_char_arr_length(symbols->terminals)-1);
		symbol = symbols->terminals[n];
	} else {
		// 50% chance it will be a terminal if max depth has
		// not been reached
		if (!full && get_randint(0, 1)) {
			n = rand_index(get_char_arr_length(symbols->terminals)-1);
			symbol = symbols->terminals[n];
		} else {
			// Pick a random function
			n = rand_index(get_char_arr_length(symbols->functions)-1);
			symbol = symbols->functions[n];
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

	hashmap_put(h, "+", 2);
	hashmap_put(h, "*", 2);
	hashmap_put(h, "/", 2);
	hashmap_put(h, "-", 2);
	hashmap_put(h, "0", 0);
	hashmap_put(h, "1", 0);

	return h;
}


/**
* Temporary function to get parameters. Will modify to read from
* a config document.
*/
struct hashmap *get_params() {
	struct hashmap *h = malloc(sizeof(struct hashmap));
	hashmap_init(h);

	hashmap_put(h, "population_size", 100);
	hashmap_put(h, "max_depth", 2);
	hashmap_put(h, "elite_size", 2);
	hashmap_put(h, "generations", 20);
	hashmap_put(h, "tournament_size", 3);
	hashmap_put(h, "seed", 0);
	hashmap_put(h, "crossover_probability", 0.8);
	hashmap_put(h, "mutation_probability", 0.2);
	hashmap_put(h, "test_tain_split", 0.7);

	return h;
}


main() {
	symbols.functions = get_functions();
	symbols.terminals = get_terminals();
	symbols.arities = get_arities();
	struct hashmap *params = get_params();

	struct node *root = new_node('+', NULL, NULL);
	root->left = new_node('*', NULL, NULL);
	root->right = new_node('2', NULL, NULL);
	root->left->left = new_node('6', NULL, NULL);
	root->left->right = new_node('*', NULL, NULL);
	root->left->right->left = new_node('5', NULL, NULL);
	root->left->right->right = new_node('7', NULL, NULL);

	struct node *test = new_node('+', NULL, NULL);

	grow(test, 0, (int)hashmap_get(params, "max_depth"), true, &symbols);
	subtree_mutation(&root, params, &symbols);
}
