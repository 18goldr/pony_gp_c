#include <stdbool.h>

#define DEFAULT_FITNESS -DBL_MAX
#define PRINT_SPACE 4

/**
* A struct to contain the different functions, terminals, and arities.
*	terminals: An array of constant numbers that can be used in the program.
*	functions: An array of functions that can be used in the program.
*	  arities: A hashmap containing the arities of different symbols
*			   to be used in the program.
*/
struct symbols {
	char *terminals;
	char *functions;
	struct hashmap *arities;
};

/**
* An individual is a struct that contains:
*		 genome: A tree.
*		fitness: The fitness of the evaluated tree.
*/
struct individual {
	struct node *genome;
	double fitness;
};

/**
* A struct to contain the data collected from the CSV file.
*	        test_cases: The test cases.
*	      test_targets: The test targets.
*	    training_cases: The training cases.
*	  training_targets: The training targets.
*/
struct csv_data {
	double **test_cases;
	double *test_targets;
	double **training_cases;
	double *training_targets;
};

void add_constants_from_csv(struct hashmap *params);
struct hashmap **parse_config(void); //util
char *get_config_file(void); //util
char *individual_to_string(struct individual i); //pony_gp
void print_individuals(struct individual *individuals, int size); //pony_gp
char get_random_symbol(int curr_depth, int max_depth, bool full, struct symbols *symbols); //pony_gp
char *get_fitness_file(void); //util
double ***parse_exemplars(char *file_name); //util
struct csv_data *get_test_and_train_data(char *file_name, double split); //util
int best_ever_index(struct individual *individuals, int size); //pony_gp
int fitness_comp(const void *elem1, const void *elem2); //pony_gp
void sort_population(struct individual **population, struct hashmap *params); //pony_gp
void print_stats(int generation, struct individual *individuals, int size, double duration, struct hashmap *params); //pony_gp
void exit_and_cleanup(struct symbols *symbols, struct hashmap *params, struct csv_data *data); //pony_gp
int double_length(double d, int d_places); //util
struct individual *new_individual(struct node *genome, double fitness); //pony_gp
bool symbol_is_valid(char sym, struct hashmap *arities);
struct symbols *get_symbols(struct hashmap *arities);
struct hashmap *get_arities(void);
struct hashmap *get_params(void);