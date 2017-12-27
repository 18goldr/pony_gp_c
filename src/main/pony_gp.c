#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

/**
* Implementation of Genetic Programming(GP), the purpose of this
* code is to describe how the algorithm works. The intended use is for
* teaching. The design is supposed to be simple, self contained and use
* core C libraries.
*
* Genetic Programming
* ===================
* An individual is a struct with two keys :
*   -genome  -- A tree
*   -fitness -- The fitness of the evaluated tree
* The fitness is maximized.
* The nodes in a GP tree consist of different symbols. The symbols are either
* functions (internal nodes with arity > 0) or terminals (leaf nodes with arity = 0) 
* The symbols are represented as a struct with the keys:
*   -arities   -- A hashmap where a key is a symbol and the value is the arity
*   -terminals -- An of characters (symbols) with arity 0
*   -functions -- A list of characters (symbols) with arity > 0
*
* Fitness Function
* ----------------
* Find a symbolic expression (function) which yields the lowest error
* for a given set of inputs.
* Inputs have explanatory variables that have a corresponding
* output. The input data is split into test and training data.The
* training data is used to generate symbolic expressions and the test
* data is used to evaluate the out - of - sample performance of the
* evaluated expressions.
*
* Pony GP Parameters
* ------------------
* The parameters for Pony GP are in a hashmap.
* 
* code author :: Robert Gold <18goldr@my.weston.org>
* original project author :: Erik Hemberg <hembergerik@csail.mit.edu>
*/

#include <stdlib.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include "util.h"
#include "binary_tree.h"
#include "queue.h"
#include "hashmap.h"
#include "csv_reader.h"

#define DEFAULT_FITNESS -DBL_MAX
#define PRINT_SPACE 4
#define MAX_ARRAY_LENGTH 2048

/**
* An individual is a struct that contains:
*		 genome: A tree.
*		fitness: The fitness of the evaluated tree.
*/
struct individual {
	struct node *genome;
	double fitness;
};

// User defined program parameters
struct hashmap *params;

// CSV data
struct csv_data *data;

// Symbols to use in the program
struct symbols *symbols;

char *individual_to_string(struct individual i);
void free_individual(struct individual *i);
void print_individuals(struct individual **individuals, int size);
char get_random_symbol(int curr_depth, int max_depth, bool full);
int best_ever_index(struct individual **individuals, int size);
int fitness_comp(const void *elem1, const void *elem2);
void sort_population(struct individual **population);
void print_stats(int generation, struct individual **individuals, int size, double duration, struct hashmap *params);
struct individual *new_individual(struct node *genome, double fitness);
void grow(struct node *node, int curr_depth, int max_depth, bool full);
void subtree_mutation(struct node **node);
struct node **subtree_crossover(struct node *parent1, struct node *parent2);
void init_population(struct individual **population, int pop_size);
double evaluate(struct node *node, double *fitness_case);
void evaluate_individual(struct individual *individual);
void evaluate_fitness(struct individual **individuals, int size, struct hashmap *cache);
struct individual **tournament_selection(struct individual **population);
struct individual **generational_replacement(struct individual **new_population, struct individual **old_population, int size);
struct individual *search_loop(struct individual **population);
struct individual *run(struct individual **population, int pop_size);
void out_of_sample_test(struct individual *individual);
void setup(void);
void cleanup(void);


/**
* Return a new individual.
*	  genome: The individuals tree.
*	 fitness: The fitness evaluation of the genome.
*/
struct individual *new_individual(struct node *genome, double fitness) {
	struct individual *new_ind = malloc(sizeof(struct individual));

	new_ind->genome = genome;
	new_ind->fitness = fitness;

	return new_ind;
}

/**
* Deallocate the individual.
*	i: The individual to deallocate.
*/
void free_individual(struct individual *i) {
	free_node(i->genome);
	free(i);
}

/**
* Return a string representation of an individual.
*	 i: The individual.
*/
char *individual_to_string(struct individual i) {
	char *genome = tree_to_string(i.genome);
	char *fitness_str = double_to_string(i.fitness, "%.4f", 4);

	int genome_length = (int)strlen(genome);
	int str_outline_len = (int)strlen("Genome: , Fitness: ,\n");
	int fitness_length = (int)strlen(fitness_str);

	char *str = malloc(genome_length + str_outline_len + fitness_length + 1); // Leave space for '\0'

	sprintf(str, "Genome: %s, Fitness: %s\n", genome, fitness_str);

	free(fitness_str);
	free(genome);

	return str;
}

/**
* Print an array of individuals.
*	 individuals: The array to print.
*	        size: The size of the array.
*/
void print_individuals(struct individual **individuals, int size) {
	for (int i = 0; i < size; i++) {
		printf("Genome: %s, Fitness: %f\n", tree_to_string(individuals[i]->genome), individuals[i]->fitness);
	}
}

/**
* Return a randomly chosen symbol. The depth determines if a terminal
* must be chosen. If `full` is specified a function will be chosen
* until the max depth is reached. The symbol is picked with a uniform
* probability.
*	   curr_depth: The current depth. Must be passed in as 0 to perform
*				   as intended.
*	    max_depth: The max depth that the tree is allowed to reach.
*	         full: True if function symbols should be used until max depth
*				   is reached.
*/
char get_random_symbol(int curr_depth, int max_depth, bool full) {
	char symbol;
	int n;

	// Pick a terminal if max depth has been reached
	if (curr_depth >= (max_depth - 1)) {
		n = rand_index(get_char_arr_length(symbols->terminals) - 1);
		symbol = symbols->terminals[n];
	}
	else {
		// 50% chance it will be a terminal if max depth has
		// not been reached
		if (!full && get_randint(0, 1)) {
			n = rand_index(get_char_arr_length(symbols->terminals) - 1);
			symbol = symbols->terminals[n];

		}
		else {
			// Pick a random function
			n = rand_index(get_char_arr_length(symbols->functions) - 1);
			symbol = symbols->functions[n];
		}
	}

	assert(symbol_is_valid(symbol, symbols->arities));

	return symbol;
}

/**
* Return the index of the individual with the best fitness.
*	  individuals: The population.
*			 size: Size of the population.
*/
int best_ever_index(struct individual **individuals, int size) {
	double best_fitness = individuals[0]->fitness;
	int best_index = 0;

	for (int i = 1; i < size; i++) {
		if (individuals[i]->fitness > best_fitness) {
			best_fitness = individuals[i]->fitness;
			best_index = i;
		}
	}

	return best_index;
}

/**
* Helper function to compare individuals in terms of their fitnessses.
* Use with qsort().
*	 elem1: The first individual to compare.
*	 elem2: The second individual.
*/
int fitness_comp(const void *elem1, const void *elem2) {
	struct individual *i1 = *(struct individual **)elem1;
	struct individual *i2 = *(struct individual **)elem2;

	if (i1->fitness < i2->fitness) return 1;
	if (i1->fitness > i2->fitness) return -1;

	return 0;
}

/**
* Sort population in reverse order in terms of fitness.
*	 population: The population to sort.
*/
void sort_population(struct individual **population) {
	int pop_size = (int)hashmap_get(params, "population_size");
	qsort(population, pop_size, sizeof(*population), fitness_comp);
}

/**
* Print the statistics for the generation and population.
*	  generation: The generation number.
*	 individuals: Population to print statistics for.
*	    duration: Duration of computation.
*	      params: User parameters.
*/
void print_stats(int generation, struct individual **individuals,
	int size, double duration, struct hashmap *params) {

	sort_population(individuals);

	if (hashmap_get(params, "verbose")) {
		printf("-------------POPULATION:-------------\n");
		print_individuals(individuals, size);
	}

	double *fitness_values = malloc(sizeof(double) * size);
	double *size_values = malloc(sizeof(double) * size);
	double *depth_values = malloc(sizeof(double) * size);

	for (int i = 0; i < size; i++) {
		fitness_values[i] = individuals[i]->fitness;
		size_values[i] = (double)get_number_of_nodes(individuals[i]->genome);
		depth_values[i] = (double)get_max_tree_depth(individuals[i]->genome);
	}

	double *fit_stats = get_ave_and_std(fitness_values, size);
	double *size_stats = get_ave_and_std(size_values, size);
	double *depth_stats = get_ave_and_std(depth_values, size);

	int max_size = (int)max_value(size_values, size);
	int max_depth = (int)max_value(depth_values, size);
	double max_fitness = max_value(fitness_values, size);

	char *ind_string = individual_to_string(*individuals[0]);

	printf(
		"Generation: %d, " 
		"Duration: %.4f, " 
		"fit ave: %.2f+/-%.3f, " 
		"size ave: %.2f+/-%.3f "
		"depth ave: %.2f+/-%.3f, " 
		"max size: %d, "
		"max depth: %d, "
		"max fit:%f "
		"best solution: %s",
		generation, 
		duration, 
		fit_stats[0], fit_stats[1], size_stats[0], size_stats[1],
		depth_stats[0], depth_stats[1], max_size, max_depth, max_fitness, ind_string
	);

	free(fitness_values);
	free(size_values);
	free(depth_values);
	free(fit_stats);
	free(size_stats);
	free(depth_stats);
	free(ind_string);
}

/*
* Recursively grow a node to max depth in a pre-order, ie. depth-first
* left-to-right traversal.
*		       node: The root node of the subtree.
*	     curr_depth: The current depth in the tree. Must be passed in as 0 to work as
*				     intended.
*	 max_tree_depth: The current max depth that the function has reached.
*	                 Must be passed in initially as 0 to work as intended.
*	      must_fill: Grows the tree to max depth when true.
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

		symbol = get_random_symbol(curr_depth, max_depth, must_fill);

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
		node_depth, (int)hashmap_get(params, "max_depth") - node_depth, false
	);

	struct node *new_subtree = new_node(new_symbol);

	// Grow the subtree
	grow(new_subtree, node_depth, (int)hashmap_get(params, "max_depth"), false);

	// Replace the old subtree with the new one
	*old_node = *new_subtree;
}

/**
* Swap two random nodes from the parents and swap the subtrees.
*	 p1: Parent to crossover.
*	 p2: Other parent to crossover.
*/
struct node **subtree_crossover(struct node *p1, struct node *p2) {
	struct node *parent1 = tree_deep_copy(p1);
	struct node *parent2 = tree_deep_copy(p2);

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
		} else {
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
*	population: The population to initialize.
*	  pop_size: The size of the population.
*/
void init_population(struct individual **population, int pop_size) {
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
		symbol = get_random_symbol(0, max_depth, full);
		tree = new_node(symbol);

		// Grow the tree if the root is a function symbol.
		if (max_depth > 0 && strchr(symbols->functions, symbol)) {
			grow(tree, 0, max_depth, full);

			assert(get_max_tree_depth(tree) < max_depth + 1);
		}

		struct individual *individual = new_individual(tree, DEFAULT_FITNESS);

		population[i] = individual;
	}
}

/**
* Evaluate a node recursively. The node's symbol (char) is evaluated.
*	         node: The node to evaluate.
*	 fitness_case: Current fitness case to test against.
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
* cases are a set of exemplars (input and ouput points) by comparing 
* the error between the output of an individual (symbolic expression)
* and the target values. Evaluates and sets the fitness in an individual.
* Fitness is the negative mean square error (MSE).
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

	// Get the mean fitness and assign it to the individual.
	individual->fitness = (fitness * -1) / targs_len;

	assert(individual->fitness <= 0);
}

/**
* Evaluate each individual of the population.
* Uses a simple cache for reducing number evaluations of individuals.
*	 individuals: Population to evaluate.
*	        size: Population size.
*		   cache: Cache for fitness evaluation.
*/
void evaluate_fitness(struct individual **individuals, int size, struct hashmap *cache) {
	for (int i = 0; i < size; i++) {
		char *key = tree_to_string(individuals[i]->genome);
		double fitness = hashmap_get(cache, key);

		if (!isnan(fitness)) {
			individuals[i]->fitness = fitness;
		}
		else {
			evaluate_individual(individuals[i]);
			hashmap_put(cache, key, individuals[i]->fitness);
		}
	}
}

/**
* Return individuals from a population by drawing `tournament_size` 
* competitors randomly and selecting the best of the competitors. 
* `population_size` number of tournaments are held.
*	 population: Population to select from
*/
struct individual **tournament_selection(struct individual **population) {
	int pop_size = (int)hashmap_get(params, "population_size");
	struct individual **winners = malloc(sizeof(struct individual *) * pop_size);

	int win_i = 0;
	struct individual *competitors[MAX_ARRAY_LENGTH];
	int tourn_size;
	int *samples;

	while (win_i < pop_size) {
		tourn_size = (int)hashmap_get(params, "tournament_size");

		samples = random_sample(pop_size, tourn_size);

		for (int i = 0; i < tourn_size; i++) {
			competitors[i] = population[samples[i]];
		}

		winners[win_i++] = population[best_ever_index(competitors, tourn_size)];

		free(samples);
	}

	return winners;
}

/**
* Return a new population. The `elite_size` best of the old_population
* replace the `elite_size` worst of the new_population, if their fitness
* is higher.
*	new_population: The new population.
*	old_population: The old population.
*			  size: The size of the new population.
*/
struct individual **generational_replacement(struct individual **new_population,
	struct individual **old_population, int size) {

	int elite_size = (int)hashmap_get(params, "elite_size");

	sort_population(new_population);
	sort_population(old_population);

	int i;

	for (i = 0; i < elite_size; i++) {
		if (old_population[i]->fitness > new_population[size - i - 1]->fitness) {
			free_individual(new_population[size - i - 1]); // free the ones that are not used
			new_population[size - i - 1] = old_population[i];
		}
	}

	// Temporary fix
	struct individual **pop = malloc(sizeof(struct individual *) * size);

	for (int i = 0; i < size; i++) {
		pop[i] = new_population[i];
	}

	return pop;
}

/**
* Return the best individual from the evolutionary search
* loop. Starting from the intitial population.
*	 population: Initial population of individuals.
*/
struct individual *search_loop(struct individual **population) {

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
	sort_population(population);
	struct individual *best_ever = population[0];

	int generation = 1;

	struct individual **new_population = malloc(sizeof(struct individual *) * pop_size);
	struct individual **parents;

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

			struct individual *parent1 = parents[rand_idxs[0]];
			struct individual *parent2 = parents[rand_idxs[1]];

			// Generate nodes by crossing over the parents
			struct node **children = subtree_crossover(parent1->genome, parent2->genome);

			struct node *child1 = children[0];
			struct node *child2 = children[1];

			free(children);

			// Append the child to the new population
			struct individual *i1 = new_individual(child1, DEFAULT_FITNESS);
			evaluate_individual(i1);
			new_population[new_pop_i++] = i1;

			// Ensure that too many elements can't be added.
			// Handles uneven population sizes, since crossover returns 2 offspring
			if (new_pop_i < pop_size) {
				struct individual *i2 = new_individual(child2, DEFAULT_FITNESS);
				evaluate_individual(i2);
				new_population[new_pop_i++] = i2;
			}
		}

		// Vary the population by mutation
		for (int i = 0; i < pop_size; i++) {
			subtree_mutation(&new_population[i]->genome);
		}

		////////////////////
		//Evaluate fitness//
		////////////////////
		evaluate_fitness(new_population, pop_size, cache);

		/////////////////////////////////////////////////////////////////
		// Replacement. Replace individual solutions in the population //
		/////////////////////////////////////////////////////////////////
		population = generational_replacement(new_population, population, pop_size);

		// Set best solution
		sort_population(population);
		best_ever = population[0];

		// Print the stats of the population
		print_stats(generation, population, pop_size, difftime(time(NULL), tic), params);

		// Increase the generation counter
		generation++;

	}

	hashmap_deinit(cache);
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
	struct hashmap **config = parse_config();
	symbols = get_symbols(config[0]);
	params = config[1];

	set_seed(hashmap_get(params, "seed"));

	data = get_test_and_train_data(get_fitness_file(), hashmap_get(params, "test_train_split"));
}

/**
* Return the best solution. Initialize a population. 
* Perform an evolutionary search.
*	population: The population to initialize.
*	  pop_size: The size of the population.
*/
struct individual *run(struct individual **population, int pop_size) {
	init_population(population, pop_size);

	struct individual *best_ever = search_loop(population);

	return best_ever;
}

/**
* Search. Evaluate best solution on out-of-sample data.
*/
int main() {
	setup();

	int pop_size = (int)hashmap_get(params, "population_size");

	struct individual **population = malloc(sizeof(struct individual *) * pop_size);
	struct individual *best_ever = run(population, pop_size);

	char *best_ever_string = individual_to_string(*best_ever);

	printf("Best solution on train data: %s\n", best_ever_string);
	out_of_sample_test(best_ever);

	free_individual(best_ever);
	free(best_ever_string);

	for (int i = 0; i < pop_size; i++) {
		free_individual(population[i]);
	}

	free(population);

	cleanup();
	exit(EXIT_SUCCESS);
}

/**
* Wrapper for deallocating memory and exiting.
*/
void cleanup() {
	free(symbols->arities);
	free(symbols->functions);
	free(symbols->terminals);
	free(symbols);
	free(params);
	free(data);

}
