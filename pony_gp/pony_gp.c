#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include "binary_tree.h"
#include "util.h"
#include "hashmap.h"
#include "csv_reader.h"
#include "queue.h"
#include "pony_gp_util.h"

// User defined program parameters
struct hashmap *params;

// CSV data
struct csv_data *data;

// Symbols to use in the program
struct symbols *symbols;

void grow(struct node *node, int curr_depth, int max_depth, bool full);
void subtree_mutation(struct node **node);
struct node **subtree_crossover(struct node *parent1, struct node *parent2);
struct individual *init_population(void);
double evaluate(struct node *node, double *fitness_case);
void evaluate_individual(struct individual *individual);
void evaluate_fitness(struct individual *individuals, int size, struct hashmap *cache);
struct individual *tournament_selection(struct individual *population);
struct individual *generational_replacement(struct individual *new_population, int size, struct individual *old_population);
struct individual *search_loop(struct individual *population);
struct individual run(void);
void out_of_sample_test(struct individual *individual);
void setup(void);
//int test_grow(void);
//int test_subtree_mutation(void);
//int test_subtree_crossover(void);
//int test(void);

/*
* Recursively grow a node to max depth in a pre-order, ie. depth-first
* left-to-right traversal.
*		      *node: The root node of the subtree.
*	     curr_depth: The current depth in the tree. Must be passed in as 0 to work as
*				     intended.
*	 max_tree_depth: The current max depth that the function has reached.
*	                 Must be passed in initially as 0 to work as intended.
*	    should_fill: Grows the tree to max depth when true.
*/
void grow(struct node *node, int curr_depth, int max_depth, bool must_fill) {
	char node_symbol = node->value;
	char node_arr[2];
	node_arr[0] = node_symbol;
	node_arr[1] = '\0';

	char symbol;

	if (curr_depth >= max_depth) return;

	// grow is called recursively in the loop. The loop iterates
	// "arity" number of times, given by the node symbol.
	for (int side = 0; side < hashmap_get(symbols->arities, node_arr); side++) {
		
		symbol = get_random_symbol(curr_depth, max_depth, must_fill, symbols);

		// new_node is not set with append_node() because it returns a copy of
		// the pointer to the new node, not the new node itself.
		append_node(node, symbol, side);
		struct node *new_node = (!side ? node->left : node->right);

		// call grow with the child node as the current node.
		grow(new_node, curr_depth+1, max_depth, must_fill);

	}

	assert(curr_depth <= max_depth);
}

/**
* Subtree mutation. Pick a node and grow it.
*	   **root: The root node of the tree.
*/
void subtree_mutation(struct node **root) {

	// Check if mutation should be applied
	if (get_rand_probability() >= hashmap_get(params, "mutation_probability")) return;

	// Pick a node
	int end_node_i = get_number_of_nodes(*root) - 1;
	int node_i = get_randint(0, end_node_i);
	struct node *old_node = get_node_at_index(root, node_i);

	// Get new subtree
	int node_depth = get_depth_at_index(*root, node_i);
		
	char new_symbol = get_random_symbol(
			node_depth, (int)hashmap_get(params, "max_depth") - node_depth, false, symbols
	);

	struct node *new_subtree = new_node(new_symbol, NULL, NULL);

	// Grow the subtree
	grow(new_subtree, 0, (int)hashmap_get(params, "max_depth"), false);

	// Replace the old subtree with the new one
	*old_node = *new_subtree;
	int x = 0;
}

/**
* Swap two random nodes from the parents and swapping the subtrees.
*		**parent1: Parent to crossover
*		**parent2: Other parent to crossover
*/
struct node **subtree_crossover(struct node *parent1, struct node *parent2) {
	parent1 = tree_deep_copy(parent1);
	parent2 = tree_deep_copy(parent2);

	// Check if crossover will occur
	if (get_rand_probability() < hashmap_get(params, "crossover_probability")) {

		struct node *xo_nodes[2];
		int node_depths[2][2];

		for (int i = 0; i < 2; i++) {
			struct node *parent = (i ? parent2 : parent1);

			// Pick a crossover point
			int end_node_i = get_number_of_nodes(parent) - 1;
			int node_i = get_randint(0, end_node_i);

			// Find the subtree at the crossover point
			xo_nodes[i] = get_node_at_index(&parent, node_i);

			node_depths[i][0] = get_depth_at_index(parent, node_i);
			node_depths[i][1] = get_max_tree_depth(xo_nodes[i]);
		}

		int max_depth = (int)hashmap_get(params, "max_depth");

		// Make sure the trees will not exceed the max depth
		if ((node_depths[0][1] + node_depths[1][0] > max_depth) ||
			(node_depths[1][1] + node_depths[0][0] > max_depth)) {

			if (hashmap_get(params, "verbose")) {
				printf("\n-----------------Crossover too deep-----------------\n");
			}
		}
		else {
			// Swap the nodes
			struct node tmp = *xo_nodes[0];

			*xo_nodes[0] = *xo_nodes[1];
			*xo_nodes[1] = tmp;

			assert(
				get_max_tree_depth(xo_nodes[0]) <= max_depth &&
				get_max_tree_depth(xo_nodes[1]) <= max_depth
			);
		}

	}

	struct node **parents = malloc(sizeof(struct node *) * 2);
	parents[0] = parent1;
	parents[1] = parent2;

	return parents;
}

/**
* Ramped half-half initialization. The individuals in the population
* are initialized using the grow or the full method for each depth
* value (ramped) up to max_depth.
*/
struct individual *init_population() {
	int pop_size = (int)hashmap_get(params, "population_size");

	struct individual *population = malloc(sizeof(struct node) * pop_size);

	for (int i = 0; i < pop_size; i++) {
		
		// Pick full or grow method
		bool full = get_randint(0, 1);

		// Ramp the depth
		int max_depth = (i % (int)hashmap_get(params, "max_depth")) + 1;

		// Create the root node
		char symbol = get_random_symbol(0, max_depth, full, symbols);
		struct node *tree = default_new_node(symbol);

		// Grow the tree if the root is a function symbol.
		if (max_depth > 0 && strchr(symbols->functions, symbol)) {
			grow(tree, 0, max_depth, full);
			assert(get_max_tree_depth(tree) < max_depth + 1);
		}

		struct individual *individual = new_individual(tree, DEFAULT_FITNESS);

		population[i] = *individual;
	}

	return population;
}

/**
* Evaluate a node recursively. The node's symbol char is evaluated.
*	*node: The evaluate node.
*	*fitness_case: Current fitness case.
*/
double evaluate(struct node *node, double *fitness_case) {

	if (!node) return -DBL_MAX;

	char symbol = node->value;

	if (symbol == '+') {
		return evaluate(node->left, fitness_case) + evaluate(node->right, fitness_case);
	}

	else if (symbol == '-') {
		return evaluate(node->left, fitness_case) - evaluate(node->right, fitness_case);
	}

	else if (symbol == '*') {
		return evaluate(node->left, fitness_case) * evaluate(node->right, fitness_case);
	}

	else if (symbol == '/') {
		double numerator = evaluate(node->left, fitness_case);
		double denominator = evaluate(node->right, fitness_case);

		if (fabs(denominator) < 0.00001) {
			denominator = 1.0;
		}
		return numerator / denominator;
	}

	else if (isalpha(symbol)) {

		// Fitness case variables must be in alphabetical order
		// for this to work correctly.
		if (symbol >= 'A') {
			return fitness_case[symbol - 'a'];
		}
		else {
			return fitness_case[symbol - 'A'];
		}
	}

	else {
		return (double)symbol - '0';
	}
}

/**
* Evaulate fitness based on fitness cases and target values. Fitness
* cases are a set of exemplars (input and ouput points) by
* comparing the error between the output of an individual (symbolic
* expression) and the target values.
* Evaluates and sets the fitness in an individual. Fitness is the
* negative mean square error (MSE).
*	     *individual: The individual solution to evaluate.
*	 **fitness_cases: Input for the evaluation.
*	        *targets: Output corresponding to the input.
*/
void evaluate_individual(struct individual *individual) {
	// Initial fitness value
	double fitness = 0.0;
	int targs_len = get_double_arr_length(data->training_targets);

	// Calculate the error between the output of the individual solution and
	// the target for each input.
	for (int i = 0; i < targs_len; i++) {
		double output = evaluate(individual->genome,data->training_cases[i]);
		
		// Get the squared error
		double error = output -data->training_targets[i];
		fitness += error * error;
	}

	assert(fitness >= 0);

	// Get the mean fitness and assign it to the individual.
	individual->fitness = (fitness * -1) / targs_len;

	assert(individual->fitness <= 0);
}

/**
* Evaluate each individual of the population.
* Uses a simple cache for reducing unmber evaluations of individuals.
*	 *individuals: Population to evaluate.
*	         size: Population size.
*		   *cache: Cache for fitness evaluation.
*/
void evaluate_fitness(struct individual *individuals, int size, struct hashmap *cache) {
	for (int i = 0; i < size; i++) {

		char *key = tree_to_string(individuals[i].genome);
		double fitness = hashmap_get(cache, key);

		if (!isnan(fitness)) {
			individuals[i].fitness = fitness;
		}
		else {
			evaluate_individual(&individuals[i]);
			hashmap_put(cache, key, individuals[i].fitness);
		}
	}
}

/**
* Return individuals from a population by drawing
* `tournament_size` competitors randomly and selecting the best
* of the competitors. `population_size` number of tournaments are
* held.
*	 *population: Population to select from
*/
struct individual *tournament_selection(struct individual *population) {
	int pop_size = (int)hashmap_get(params, "population_size");
	struct individual *winners = malloc(sizeof(struct individual) * pop_size);

	int win_i = 0;
	struct individual *competitors;
	int tourn_size;
	int *samples;

	while (win_i < pop_size) {
		tourn_size = (int)hashmap_get(params, "tournament_size");
		competitors = malloc(sizeof(struct individual) * tourn_size);

		samples = random_sample(pop_size, tourn_size);

		for (int i = 0; i < tourn_size; i++) {
			competitors[i] = population[samples[i]];
		}

		winners[win_i++] = population[best_ever_index(competitors, tourn_size)];
	}

	free(samples);

	return winners;
}

/**
* Return a new population. The `elite_size` best old_population
* are appended to the new population. They are kept in the new
* population if they are better than the worst.
*/
struct individual *generational_replacement(struct individual *new_population,
	int size, struct individual *old_population) {

	int elite_size = (int)hashmap_get(params, "elite_size");

	struct individual *population = malloc(sizeof(struct individual) * (elite_size + size));

	int i;

	for (i = 0; i < size; i++) {
		population[i] = new_population[i];
	}

	for (i; i < elite_size; i++) {
		population[i] = old_population[i];
	}

	sort_population(&population, params);

	population = realloc(population, sizeof(struct individual) * size);

	return population;
}

/**
* Return the best individual from the evolutionary search
* loop. Starting from the intitial popualtion.
*	 *population: Initial population of individuals.
*/
struct individual *search_loop(struct individual *population) {

	/////////////////////
	//Evaluate Fitness //
	/////////////////////
	struct hashmap *cache = hashmap_init();
	time_t tic = time(NULL);
	int pop_size = (int)hashmap_get(params, "population_size");
	int tourn_size = (int)hashmap_get(params, "tournament_size");
	evaluate_fitness(population, pop_size, cache);

	// Print stats of the population
	print_stats(0, population, pop_size, difftime(time(NULL), tic), params);

	// Set best solution
	sort_population(&population, params);
	struct individual *best_ever = &population[0];

	int generation = 1;

	struct individual *new_population = malloc(sizeof(struct individual) * pop_size);
	struct individual *parents;

	/////////////////////
	// Generation Loop //
	/////////////////////

	while (generation < hashmap_get(params, "generations")) {
		tic = time(NULL);

		int new_pop_i = 0;

		parents = tournament_selection(population);

		while (new_pop_i < pop_size) {
			int *rand_idxs = random_sample(pop_size, 2);

			struct individual parent1 = parents[rand_idxs[0]];
			struct individual parent2 = parents[rand_idxs[1]];

			struct node *node1 = parent1.genome;
			struct node *node2 = parent2.genome;

			struct node **nodes = subtree_crossover(node1, node2);
			node1 = nodes[0];
			node2 = nodes[1];

			struct individual i1 = *new_individual(node1, parent1.fitness);
			new_population[new_pop_i++] = i1;

			if (new_pop_i < pop_size) {
				struct individual i2 = *new_individual(node2, parent2.fitness);
				new_population[new_pop_i++] = i2;
			}
		}

		for (int i = 0; i < pop_size; i++) {
			subtree_mutation(&new_population[i].genome);
		}

		evaluate_fitness(new_population, pop_size, cache);

		population = generational_replacement(new_population, pop_size, population);

		sort_population(&population, params);
		best_ever = &population[0];

		print_stats(generation, population, pop_size, difftime(time(NULL), tic), params);

		generation++;
	
	}

	free(cache);
	free(parents);
	free(new_population);

	return best_ever;
}

/**
* Out-of-sample test on an individual soltuion.
*	 *individual: Solution to test on data.
*/
void out_of_sample_test(struct individual *individual) {
	evaluate_individual(individual);

	printf("Best solution of test data: %s\n", individual_to_string(*individual));
}

/**
* Wrapper for setup.
*/
void setup() {
	symbols = get_symbols();
	params = get_params();
	set_seed(hashmap_get(params, "seed"));

	data = get_test_and_train_data(get_fitness_file(), hashmap_get(params, "test_train_split"));
}


/**
* Return the best solution. Create an initial
* population. perform an evolutionary search.
*/
struct individual run() {
	struct individual *population = init_population();

	struct individual *best_ever = search_loop(population);

	free(population);

	return *best_ever;
}

main() {
	setup();

	struct individual *population = init_population();

	struct individual *best_ever = search_loop(population);
	printf("Best solution on train data: %s\n", individual_to_string(*best_ever));

	out_of_sample_test(best_ever);

	exit_and_cleanup(symbols, params, data);
}
//
///**
//* Run all tests on current seed.
//*/
//int test() {
//	if (test_grow() || test_subtree_mutation() || test_subtree_crossover()) {
//		return (EXIT_FAILURE);
//	}
//
//	return (EXIT_SUCCESS);
//	
//}
//
///**
//* A function to test the grow function.
//* Valid seeds: -24, -5, -3, 1, 2, 4, 7
//* Max depth must be 3.
//* Return EXIT_FAILURE if seed provided is invalid or max depth is not 3.
//*/
//int test_grow() {
//	if (hashmap_get(params, "max_depth") != 3) {
//		printf("Please set max_depth to 3");
//		return(EXIT_FAILURE);
//	}
//
//	struct node *test = new_node('+', NULL, NULL);
//	grow(test, 0, (int)hashmap_get(params, "max_depth"), false);
//
//	char seed_neg24[] = { '+', '+', '+', '0', '0', '+', '0', '0', '+', '+', '0', '0', '+', '0', '0' };
//	char seed_neg5[] = { '+', '/', '/', '0', '0', '/', '0', '0', '/', '/', '0', '0', '/', '0', '0' };
//	char seed_neg3[] = { '+', '+', '+', '0', '0', '+', '0', '0', '+', '+', '0', '0', '+', '0', '0' };
//	char seed_1[] = { '+', '1', '1' };
//	char seed_2[] = { '+', '1', '1' };
//	char seed_4[] = { '+', '1', '1' };
//	char seed_7[] = { '+', '1', '1' };
//
//	char *arr_to_use;
//
//	switch ((int)hashmap_get(params, "seed")) {
//		case -3:
//			arr_to_use = seed_neg3;
//			break;
//		case -5:
//			arr_to_use = seed_neg5;
//			break;
//		case 4:
//			arr_to_use = seed_4;
//			break;
//		case 2:
//			arr_to_use = seed_2;
//			break;
//		case -24:
//			arr_to_use = seed_neg24;
//			break;
//		case 7:
//			arr_to_use = seed_7;
//			break;
//		case 1:
//			arr_to_use = seed_1;
//			break;
//		default:
//			printf("Current seed is not testable\n");
//			return(EXIT_FAILURE);
//	};
//
//	for (int i = 0; i < get_number_of_nodes(test); i++) {
//		assert(get_node_at_index(&test, i)->value == arr_to_use[i]);
//	}
//
//	free(test);
//
//	return(EXIT_SUCCESS);
//
//}
//
///**
//* A function to test the subtree_mutation function.
//* Valid seeds: -24, -5, -3, 1, 2, 4, 7
//* Max depth must be 3.
//* Return EXIT_FAILURE if seed provided is invalid or max depth is not 3.
//*/
//int test_subtree_mutation() {
//	if (hashmap_get(params, "max_depth") != 3) {
//		printf("Please set max_depth to 3");
//		return(EXIT_FAILURE);
//	}
//
//	struct node *test = new_node('+', NULL, NULL);
//	test->left = new_node('*', NULL, NULL);
//	test->right = new_node('2', NULL, NULL);
//	test->left->left = new_node('6', NULL, NULL);
//	test->left->right = new_node('*', NULL, NULL);
//	test->left->right->left = new_node('5', NULL, NULL);
//	test->left->right->right = new_node('7', NULL, NULL);
//
//	subtree_mutation(&test);
//
//	char seed_neg3[] = { '+', '+', '+', '0', '0', '+', '0', '0', '+', '+', '0', '0', '+', '0', '0' };
//	char seed_neg5[] = { '+', '0', '2' };
//	char seed_4[] = { '+', '*', '1', '*', '5', '7', '2' };
//	char seed_2[] = { '+', '*', '6', '1', '2' };
//	char seed_neg24[] = { '+', '*', '6', '0', '2' };
//	char seed_7[] = { '+', '*', '6', '*', '5', '1', '2' };
//	char seed_1[] = { '+', '*', '6', '*', '5', '7', '1' };
//
//	char *arr_to_use;
//
//	switch ((int)hashmap_get(params, "seed")) {
//		case -3:
//			arr_to_use = seed_neg3;
//			break;
//		case -5:
//			arr_to_use = seed_neg5;
//			break;
//		case 4:
//			arr_to_use = seed_4;
//			break;
//		case 2:
//			arr_to_use = seed_2;
//			break;
//		case -24:
//			arr_to_use = seed_neg24;
//			break;
//		case 7:
//			arr_to_use = seed_7;
//			break;
//		case 1:
//			arr_to_use = seed_1;
//			break;
//		default:
//			printf("Current seed is not testable\n");
//			return(EXIT_FAILURE);
//	};
//
//
//
//	for (int i = 0; i < get_number_of_nodes(test); i++) {
//		assert(get_node_at_index(&test, i)->value == arr_to_use[i]);
//	}
//
//	free(test);
//
//	return(EXIT_SUCCESS);
//
//}
//
///**
//* A function to test the subtree_crossover function.
//* Valid seeds: -24, -5, -3, 1, 2, 4, 7
//* Max depth must be 3.
//* Return EXIT_FAILURE if seed provided is invalid or max depth is not 3.
//*/
//int test_subtree_crossover() {
//	if (hashmap_get(params, "max_depth") != 3) {
//		printf("Please set max_depth to 3");
//		return(EXIT_FAILURE);
//	}
//
//	struct node *test1 = new_node('+', NULL, NULL);
//	test1->left = new_node('*', NULL, NULL);
//	test1->right = new_node('2', NULL, NULL);
//	test1->left->left = new_node('6', NULL, NULL);
//	test1->left->right = new_node('*', NULL, NULL);
//	test1->left->right->left = new_node('5', NULL, NULL);
//	test1->left->right->right = new_node('7', NULL, NULL);
//
//	struct node *test2 = new_node('-', NULL, NULL);
//	test2->left = new_node('-', NULL, NULL);
//	test2->right = new_node('1', NULL, NULL);
//	test2->left->left = new_node('3', NULL, NULL);
//	test2->left->right = new_node('/', NULL, NULL);
//	test2->left->right->left = new_node('4', NULL, NULL);
//	test2->left->right->right = new_node('8', NULL, NULL);
//
//	char seed1_neg3[] = { '-', '-', '3', '/', '4', '8', '1' };
//	char seed1_neg5[] = { '+', '-', '3', '/', '4', '8', '2' };
//	char seed1_4[] = { '+', '*', '3', '*', '5', '7', '2' };
//	char seed1_2[] = { '+', '*', '6', '/', '4', '8', '2' };
//	char seed1_neg24[] = { '+', '*', '6', '/', '4', '8', '2' };
//	char seed1_7[] = { '+', '*', '6', '*', '5', '8', '2' };
//	char seed1_1[] = { '+', '*', '6', '*', '5', '7', '1' };
//
//	char seed2_neg3[] = { '+', '*', '6', '*', '5', '7', '2' };
//	char seed2_neg5[] = { '+', '-', '3', '/', '4', '8', '2' };
//	char seed2_4[] = { '-', '-', '6', '/', '4', '8', '1' };
//	char seed2_2[] = { '-', '-', '3', '*', '5', '7', '1' };
//	char seed2_neg24[] = { '-', '-', '3', '*', '5', '7', '1' };
//	char seed2_7[] = { '-', '-', '3', '/', '4', '7', '1' };
//	char seed2_1[] = { '-', '-', '3', '/', '4', '8', '2' };
//
//	char *arr1_to_use;
//	char *arr2_to_use;
//
//	subtree_crossover(&test1, &test2);
//
//	switch ((int)hashmap_get(params, "seed")) {
//		case -3:
//			arr1_to_use = seed1_neg3;
//			arr2_to_use = seed2_neg3;
//			break;
//		case -5:
//			arr1_to_use = seed1_neg5;
//			arr2_to_use = seed2_neg5;
//			break;
//		case 4:
//			arr1_to_use = seed1_4;
//			arr2_to_use = seed2_4;
//			break;
//		case 2:
//			arr1_to_use = seed1_2;
//			arr2_to_use = seed2_2;
//			break;
//		case -24:
//			arr1_to_use = seed1_neg24;
//			arr2_to_use = seed2_neg24;
//			break;
//		case 7:
//			arr1_to_use = seed1_7;
//			arr2_to_use = seed2_7;
//			break;
//		case 1:
//			arr1_to_use = seed1_1;
//			arr2_to_use = seed2_1;
//			break;
//		default:
//			printf("Current seed is not testable\n");
//			return(EXIT_FAILURE);
//	};
//
//	for (int i = 0; i < get_number_of_nodes(test1); i++) {
//		assert(get_node_at_index(&test1, i)->value == arr1_to_use[i]);
//	}
//
//	for (int i = 0; i < get_number_of_nodes(test2); i++) {
//		assert(get_node_at_index(&test2, i)->value == arr2_to_use[i]);
//	}
//
//	free(test1);
//	free(test2);
//
//	return (EXIT_SUCCESS);
//}
//
