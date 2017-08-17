#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <math.h>
#include "binary_tree.h"
#include "util.h"
#include "hashmap.h"
#include "csv_reader.h"
#include "queue.h"
#include "pony_gp_util.h"

/**
* Temporary function to set parameters. Will modify to read from
* a config document.
*/
struct hashmap *get_params() {
	struct hashmap *h = hashmap_init();

	hashmap_put(h, "population_size", 100);
	hashmap_put(h, "max_depth", 5);
	hashmap_put(h, "elite_size", 3);
	hashmap_put(h, "generations", 100);
	hashmap_put(h, "tournament_size", 5);
	hashmap_put(h, "crossover_probability", 0.8);
	hashmap_put(h, "mutation_probability", 0.2);
	hashmap_put(h, "test_train_split", 0.7);
	hashmap_put(h, "verbose", 0);
	hashmap_put(h, "seed", 0);

	return h;
}

/**
* Edit this with the path to your csv file.
*/
char *get_fitness_file() {
	return "C:/Users/Robbie/Dropbox/MIT/pony_gp (Cmake)/pony_gp/data/fitness_cases.csv";
}

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
* Return a string representation of an individual.
*	 i: The individual.
*/
char *individual_to_string(struct individual i) {
	char *genome = tree_to_string(i.genome);
	int genome_length = (int)strlen(genome);
	int str_outline_len = (int)strlen("Genome: , Fitness: ,\n");
	
	// Number of decimal places to add
	int sprintf_d_places = 4;
	int d_len = double_length(i.fitness, sprintf_d_places) + 1; // Allow for decimal place

	if (i.fitness < 0) d_len++; // Allow for negative sign
	
	char *str = malloc(genome_length + str_outline_len + d_len + 1); // Leave space for '\0'
	
	sprintf(str, "Genome: %s, Fitness: %.4f\n", genome, i.fitness);

	return str;
}

/**
* Print an array of individuals.
*	 individuals: The array to print.
*	        size: The size of the array.
*/
void print_individuals(struct individual *individuals, int size) {
	for (int i = 0; i < size; i++) {
		struct individual ind = individuals[i];
		printf("Genome: %s, Fitness: %f\n", tree_to_string(ind.genome), ind.fitness);
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
char get_random_symbol(int curr_depth, int max_depth, bool full, struct symbols *symbols) {
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
* Return is a symbol is valid.
*	 sym: The symbol to verify.
*	 arities: Hashmap of symbols and their arities.
*/
bool symbol_is_valid(char sym, struct hashmap *arities) {
	char symbol_str[] = { sym, '\0' };
	return (!isnan(hashmap_get(arities, symbol_str)));
}

/**
* Set the symbols. Helper method keep the code clean.
*
* The nodes in a GP tree consists of different symbols. The symbols
* are either functions (internal nodes with arity > 0) or terminals
* (leaf nodes with arity = 0). The symbols are represented as a struct
* with the values:
*	     `arities`: A hashmap where a key is a symbol
*		     		and the value is the arity.
*	   `terminals`: An array of strings (symbols) with arity 0.
*	   `functions`: An array of string (symbols) with arity > 0.
*/
struct symbols *get_symbols() {
	struct symbols *symbols = malloc(sizeof(struct symbols));
	symbols->arities = get_arities();

	int num_terminals = hashmap_get_num_with_value(symbols->arities, 0);
	int size = hashmap_get_size(symbols->arities);

	char *functions = malloc((size)-num_terminals + 1);
	char *terminals = malloc(num_terminals + 1);
	int f_i = 0;
	int t_i = 0;

	for (int i = 0; i < size; i++) {
		char *key = hashmap_get_key(symbols->arities, i);
		int value = (int)hashmap_get(symbols->arities, key);

		if (!value) {
			terminals[f_i++] = *key;
		}
		else {
			functions[t_i++] = *key;
		}
	}

	functions[f_i] = '\0';
	terminals[t_i] = '\0';

	symbols->functions = functions;
	symbols->terminals = terminals;

	return symbols;
}

/**
* Temporary function to get arities. Will modify to read from
* a config document.
*/
struct hashmap *get_arities() {
	struct hashmap *h = hashmap_init();

	hashmap_put(h, "+", 2);
	hashmap_put(h, "*", 2);
	hashmap_put(h, "/", 2);
	hashmap_put(h, "-", 2);
	hashmap_put(h, "0", 0);
	hashmap_put(h, "1", 0);
	hashmap_put(h, "a", 0);
	hashmap_put(h, "b", 0);

	return h;
}



/**
* Parse a CSV file. Parse the fitness case and split the data into
* test and train data. in the fitness case file each row is an exemplar
* and each dimension is in a column. The last column is the target value
* of the exemplar.
*	 file_name: Name of CSV file with a header.
*/
double ***parse_exemplars(char *file_name) {
	csv_reader *reader = init_csv(file_name, ',');

	double **fitness_cases, *targets;
	int num_columns = get_num_column(reader);
	int num_lines = get_num_lines(reader);

	// leave space for NULL
	fitness_cases = malloc(sizeof(double *) * (num_lines));

	for (int i = 0; i < num_lines; i++) {
		fitness_cases[i] = malloc(sizeof(double) * (num_columns - 1));
	}

	// leave space for NAN
	targets = malloc(sizeof(double) * (num_lines));

	csv_line *row;
	int f_i = 0;
	int t_i = 0;

	// Ignore the header
	next_line(reader);

	while ((row = readline(reader))) {
		int i;
		for (i = 0; i < num_columns; i++) {
			if (i == num_columns - 1) {
				targets[t_i++] = atof(row->content[i]);
			}
			else {
				fitness_cases[f_i][i] = atof(row->content[i]);
			}
		}
		fitness_cases[f_i][i - 1] = NAN;
		f_i++;
	}
	fitness_cases[f_i] = NULL;
	targets[t_i] = NAN;

	double ***results = malloc(sizeof(double **) * 2);
	double *tmp[] = { targets };
	results[0] = fitness_cases;
	results[1] = tmp;

	free(row);
	free(reader);

	return results;
}

/**
* Return test and train data. Random selection or exemplars(ros)
* from file containing data.
*	  file_name: Name of CSV file with a header.
*		  split: Percentage of exemplar data used for training.
*/
struct csv_data *get_test_and_train_data(char *file_name, double split) {
	double ***exemplars = parse_exemplars(file_name);

	double **fitness = exemplars[0];
	double *targs = *exemplars[1];

	int fitness_len = get_2d_arr_length(fitness);
	int targs_len = get_double_arr_length(targs);

	int col_size = get_double_arr_length(fitness[0]);

	// randomize the index order
	int fits_split_i = (int)(floor(fitness_len * split));
	int *fits_rand_idxs = random_indexes(fitness_len);


	int targs_split_i = (int)(floor(targs_len * split));
	int *targs_rand_idxs = random_indexes(targs_len);

	double **training_cases = malloc((sizeof(double *) * fits_split_i) + 1);
	double **test_cases = malloc((sizeof(double *) * (fitness_len - fits_split_i)) + 1);

	for (int i = 0; i < 2; i++) {
		training_cases[i] = malloc(sizeof(double) * col_size);
		test_cases[i] = malloc(sizeof(double) * col_size);
	}

	double *training_targets = malloc((sizeof(double) * targs_split_i) + 1);
	double *test_targets = malloc(sizeof(double) * (targs_len - targs_split_i) + 1);

	int rand_i;
	int i;

	for (i = 0; i < fitness_len; i++) {
		rand_i = fits_rand_idxs[i];

		if (i >= fits_split_i) {
			test_cases[i - fits_split_i] = fitness[rand_i];
		}
		else {
			training_cases[i] = fitness[rand_i];
		}
	}

	for (i = 0; i < targs_len; i++) {
		rand_i = targs_rand_idxs[i];

		if (i >= targs_split_i) {
			test_targets[i - targs_split_i] = targs[rand_i];
		}
		else {
			training_targets[i] = targs[rand_i];
		}
	}

	// Set last index to NULL/NAN to allow for easier looping of arrays
	training_cases[fits_split_i] = NULL;
	test_cases[fitness_len - fits_split_i] = NULL;
	training_targets[targs_split_i] = NAN;
	test_targets[targs_len - targs_split_i] = NAN;

	struct csv_data *results = malloc(sizeof(struct csv_data));
	results->test_cases = test_cases;
	results->training_cases = training_cases;
	results->training_targets = training_targets;
	results->test_targets = test_targets;

	free(exemplars);
	free(targs_rand_idxs);
	free(fits_rand_idxs);

	return results;
}

/**
* Return the index of the individual with the best fitness.
*	  individuals: The population.
*			 size: Size of the population.
*/
int best_ever_index(struct individual *individuals, int size) {
	double best_fitness = individuals[0].fitness;
	int best_index = 0;

	for (int i = 1; i < size; i++) {
		if (individuals[i].fitness > best_fitness) {
			best_fitness = individuals[i].fitness;
			best_index = i;
		}
	}

	return best_index;
}

/**
* Helper function to compare individuals in terms of their fitnessses.
* For sorting in reverse order
* Use for qsort()
*	 elem1: The first individual to compare.
*	 elem2: The second individual.
*/
int fitness_comp(const void *elem1, const void *elem2) {
	struct individual *i1 = (struct individual *)elem1;
	struct individual *i2 = (struct individual *)elem2;

	if (i1->fitness < i2->fitness) return 1;
	if (i1->fitness > i2->fitness) return -1;
	
	return 0;
}

/**
* Sort population in reverse order in terms of fitness.
*	 population: The population to sort.
*/
void sort_population(struct individual **population, struct hashmap *params) {
	int pop_size = (int)hashmap_get(params, "population_size");
	qsort(*population, pop_size, sizeof(struct individual), fitness_comp);
}

/**
* Print the statistics for the generation and population.
*	  generation: The generation number
*	 individuals: Population to get statistics for
*	    duration: Duration of computation
*	      params: User parameters
*/
void print_stats(int generation, struct individual *individuals, 
			int size, double duration, struct hashmap *params) {
	
	sort_population(&individuals, params);

	if (hashmap_get(params, "verbose")) {
		printf("-------------POPULATION:-------------\n");
		print_individuals(individuals, size);
	}

	double *fitness_values = malloc(sizeof(double) * size);
	double *size_values = malloc(sizeof(double) * size);
	double *depth_values = malloc(sizeof(double) * size);

	for (int i = 0; i < size; i++) {
		fitness_values[i] = individuals[i].fitness;
		size_values[i] = (double)get_number_of_nodes(individuals[i].genome);
		depth_values[i] = (double)get_max_tree_depth(individuals[i].genome);
		//assert(depth_values[i] <= hashmap_get(params, "max_depth"));
	}
	
	double *fit_stats = get_ave_and_std(fitness_values, size);
	double *size_stats = get_ave_and_std(size_values, size);
	double *depth_stats = get_ave_and_std(depth_values, size);

	int max_size = (int)max_value(size_values, size);
	int max_depth = (int)max_value(depth_values, size);
	double max_fitness = max_value(fitness_values, size);

	
	char *ind_string = individual_to_string(individuals[0]);

	printf(
		"Generation: %d, Duration: %.4f, fit ave: %.2f+-%.3f, size ave: %.2f+-%.3f "
		"depth ave: %.2f+-%.3f, max size: %d, max depth: %d, max fit:%f "
		"best solution: %s",
		generation, duration, fit_stats[0], fit_stats[1], size_stats[0], size_stats[1],
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

/**
* Wrapper for deallocating memory and exiting.
*/
void exit_and_cleanup(struct symbols *symbols, struct hashmap *params, struct csv_data *data) {
	free(symbols->arities);
	free(symbols->functions);
	free(symbols->terminals);
	free(params);
	free(data);

	exit(EXIT_SUCCESS);
}
