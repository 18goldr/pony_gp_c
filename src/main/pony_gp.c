#include <stdlib.h>
#include <stdlib.h>
#include <crtdbg.h>
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
struct individual *run(void);
void out_of_sample_test(struct individual *individual);
void setup(void);

/*
* Recursively grow a node to max depth in a pre-order, ie. depth-first
* left-to-right traversal.
*		       node: The root node of the subtree.
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
		if (curr_depth + 1 < max_depth && strchr(symbols->functions, symbol)) {
			grow(new_node, curr_depth + 1, max_depth, must_fill);
		}

	}

	assert(curr_depth <= max_depth);
}

/**
* Subtree mutation. Pick a node and grow it.
*	 root: The root node of the tree.
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

	struct node *new_subtree = new_node(new_symbol);

	// Grow the subtree
	grow(new_subtree, node_depth, (int)hashmap_get(params, "max_depth"), false);

	// Replace the old subtree with the new one
	*old_node = *new_subtree;
}

/**
* Swap two random nodes from the parents and swapping the subtrees.
*	 parent1: Parent to crossover
*	 parent2: Other parent to crossover
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

	struct individual *population = malloc(sizeof(struct individual) * pop_size);

	bool full;
	int max_depth;
	char symbol;
	struct node *tree;

	for (int i = 0; i < pop_size; i++) {

		// Pick full or grow method
		full = get_randint(0, 1);

		// Ramp the depth
		max_depth = (i % (int)hashmap_get(params, "max_depth")) + 1;

		// Create the root node
		symbol = get_random_symbol(0, max_depth, full, symbols);
		tree = new_node(symbol);

		// Grow the tree if the root is a function symbol.
		if (max_depth > 0 && strchr(symbols->functions, symbol)) {
			grow(tree, 0, max_depth, full);

			assert(get_max_tree_depth(tree) < max_depth + 1);
		}

		struct individual individual = *new_individual(tree, DEFAULT_FITNESS);

		population[i] = individual;

	}

	return population;
}

/**
* Evaluate a node recursively. The node's symbol char is evaluated.
*	         node: The evaluate node.
*	 fitness_case: Current fitness case.
*/
double evaluate(struct node *node, double *fitness_case) {

	if (!node) return -DBL_MAX;

	char symbol = node->value;

	assert(symbol_is_valid(symbol, symbols->arities));

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
*	    individual: The individual solution to evaluate.
*	 fitness_cases: Input for the evaluation.
*	       targets: Output corresponding to the input.
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
*	 individuals: Population to evaluate.
*	        size: Population size.
*		   cache: Cache for fitness evaluation.
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
*	 population: Population to select from
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
*	 population: Initial population of individuals.
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

		///////////////
		// Selection //
		///////////////

		parents = tournament_selection(population);

		///////////////////////////////////////////////////
		// Varation -- Generate new individual solutions //
		///////////////////////////////////////////////////

		// Crossover
		while (new_pop_i < pop_size) {
			// Select parents
			int *rand_idxs = random_sample(pop_size, 2);

			struct individual parent1 = parents[rand_idxs[0]];
			struct individual parent2 = parents[rand_idxs[1]];

			// Generate nodes by crossing over the parents
			struct node **children = subtree_crossover(parent1.genome, parent2.genome);
			struct node *child1 = children[0];
			struct node *child2 = children[1];

			// Append the child to the new population
			struct individual i1 = *new_individual(child1, parent1.fitness);
			new_population[new_pop_i++] = i1;

			// Ensure that too many elements can't be added.
			// Handles uneven population sizes, since crossover returns 2 offspring
			if (new_pop_i < pop_size) {
				struct individual i2 = *new_individual(child2, parent2.fitness);
				new_population[new_pop_i++] = i2;
			}
		}

		// Vary the population by mutation
		for (int i = 0; i < pop_size; i++) {
			subtree_mutation(&new_population[i].genome);
		}

		////////////////////
		//Evaluate fitness//
		////////////////////
		evaluate_fitness(new_population, pop_size, cache);

		/////////////////////////////////////////////////////////////////
		// Replacement. Replace individual solutions in the population //
		/////////////////////////////////////////////////////////////////
		population = generational_replacement(new_population, pop_size, population);

		// Set best solution
		sort_population(&population, params);
		best_ever = &population[0];

		// Print the stats of the population
		print_stats(generation, population, pop_size, difftime(time(NULL), tic), params);

		// Increase the generation counter
		generation++;
	
	}

	free(cache);
	free(parents);
	free(new_population);

	return best_ever;
}

/**
* Out-of-sample test on an individual soltuion.
*	 individual: Solution to test on data.
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
* population. Perform an evolutionary search.
*/
struct individual *run() {
	struct individual *population = init_population();

	struct individual *best_ever = search_loop(population);

	free(population);

	return best_ever;
}

/**
* Search. Evaluate best solution on out-of-sample data.
*/
main() {
	setup();

	struct individual *best_ever = run();
	printf("Best solution on train data: %s\n", individual_to_string(*best_ever));

	out_of_sample_test(best_ever);

	free(best_ever);

	printf("\nPress enter to exit...");
	getchar();

	exit_and_cleanup(symbols, params, data);
}
