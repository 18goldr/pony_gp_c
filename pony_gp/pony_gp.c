#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "binary_tree.h"
#include "util.h"
#include "hashmap.h"

char get_random_symbol(int, int, struct symbols *, bool);

char *get_terminals(void); 
char *get_functions(void);
struct hashmap *get_arities(void);


/*
 * A struct to contain the different functions, terminals, and arities.
 */
struct symbols {
	char *terminals;
	char *functions;
	struct hashmap *arities;
} symbols;

/*
 * Return a randomly chosen symbol. The depth determines if a terminal
 * must be chosen. If `full` is specified a function will be chosen
 * until the max depth is reached. The symbol is picked with a uniform
 * probability.
 */
char get_random_symbol(int depth, int max_depth, struct symbols *symbols, bool full) {
	char symbol;
	int n;

	// Pick a terminal if max depth has been reached
	if (depth >= max_depth - 1) {
		n = rand_index(get_char_arr_length(symbols->terminals));
		symbol = *(symbols->terminals += n);
	}
	else {
		// 50% chance it will be a terminal if max depth has
		// not been reached
		if (!full && get_rand(0, 1)) {
			n = rand_index(get_char_arr_length(symbols->terminals)-1);
			symbol = *(symbols->terminals += n);
		}
		else {
			// Pick a random function
			n = rand_index(get_char_arr_length(symbols->functions)-1);
			symbol = *(symbols->functions += n);
		}
	}

	return symbol;
}
 

/*
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


/*
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

	struct node tree[] = {
		{'+', &tree[1], &tree[6]}, // tree[0]
		{'*', &tree[2], &tree[3]}, // tree[1]
		{'6', NULL,     NULL    }, // tree[2]
		{'*', &tree[4], &tree[5]}, // tree[3]
		{'5', NULL,     NULL    }, // tree[4]
		{'7', NULL,     NULL    }, // tree[5]
		{'2', NULL,     NULL    }  // tree[6]
	};

	struct node *test = new_node('+', NULL, NULL);

	struct node *ptr = &tree[3];
	struct node **ptr2 = &ptr;

	replace_subtree(ptr2, &test);

	printf("%c", *(&tree[3].value));
}
