#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "util.h"
#include "hashmap.h"
#include "config_parser.h"
#include "csv_reader.h"
#include "include/params.h"

#define ARR_SIZE(arr) ((int)(sizeof(arr) / sizeof(0[arr])))
#define MAX_LINE_LENGTH 2048

// the seed to pass to the random function
// default to 0
double SEED = 0;

/**
* Set the seed for the random number generator.
* Useful for testing purposes.
*	 s: Value that SEED will be set to.
*/
void set_seed(double s) {
	SEED = s;
}

/**
* Convert a double to a string.
*	 sig_decimal_places: The number of significant figures in the decimal part
*						 of the double to represent. Must match the format
*				         specifier.
*			          d: The double to convert.
*				 format: The format specifier for string interpolation.
*/
char *double_to_string(double d, char *format, int sig_decimal_places) {
	int d_len = double_length(d, sig_decimal_places) + 1; // Allow for decimal place
	
	if (d < 0) d_len++; // Allow for negative sign
	
	char *str = malloc(d_len + 1); // Allow space for null character
	sprintf(str, format, d);

	return str;
}

/**
* Convert an int to a string.
*	 i: The integer to convert.
*/
char *int_to_string(int i) {
	char *str = malloc(int_length(i));

	sprintf(str, "%d", i);

	return str;
}

/**
* Return the next non-empty line of a file.
*	file: The file to read.
*/
char *getline(FILE *file) {
	char *f_line = calloc(MAX_LINE_LENGTH, sizeof(char));

	fgets(f_line, MAX_LINE_LENGTH, file);
	// Removes newline character from fgets
	f_line[strcspn(f_line, "\n")] = 0;

	// Ignore line if blank
	while (!f_line[0]) {
		// If end of file, return NULL.
		if (!fgets(f_line, MAX_LINE_LENGTH, file)) return NULL;

		// Removes newline character from fgets
		f_line[strcspn(f_line, "\n")] = 0;
	}

	return f_line;
}

/**
* Get the length of a character array/string
* Array must end in Null character.
*	arr: The array to parse.
*/
int get_char_arr_length(char *arr) {
	int length = 1;

	while (*arr) {
		length++;
		arr++;
	}

	return length;

}

/**
* Get the length of a double array.
* Array must end in the value NAN defined in <math.h>
*	arr: The array to parse.
*/
int get_double_arr_length(double *arr) {
	int length = 0;

	while (!isnan(*arr)) {
		length++;
		arr++;
	}

	return length;
}

/**
* Get the length of a 2 dimensional array of doubles. Each outer array must end
* with a NULL pointer. Each inner array must end with the NAN value
* defined in <math.h>
*	 arr: The 2D array to parse.
*/
int get_2d_arr_length(double **arr) {
	int length = 0;

	while (*arr) {
		length++;
		arr++;
	}

	return length;
}


/**
* Call srand. Can only be performed once per execution.
*/
void start_srand() {
	// srand must only be called once for different results each time.
	static bool first_time = true;

	if (first_time) {
		srand((unsigned int)(SEED ? SEED : time(NULL)));
		first_time = false;
	}
}

/*
* Return a random int between min and max inclusive.
*	min: The minimum value.
*	max: The maxmium value.
*/
int get_randint(int min, int max) {
	start_srand();

	return (rand() % (max + 1 - min)) + min;

}


/**
* Return a random probability. Values are 0.0 to 1.0 inclusive.
*/
double get_rand_probability() {
	start_srand();

	return (double)rand() / (double)RAND_MAX;
}


/*
* Get a random index of an array.
*	length: The length of the array.
*/
int rand_index(int length) {
	return get_randint(0, length-1);
}


/**
* Return an array of random indexes.
*	 length: Length of the array.
*/
int *random_indexes(int length) {
	int *indexes = fill_array_indexes(length);

	shuffle(indexes, length);

	return indexes;
}

/**
* Return an array of a random sample of indexes.
*	      length: The length of the array.
*	 sample_size: The size of the sample array to return.
*/
int *random_sample(int length, int sample_size) {
	int *all_indexes = fill_array_indexes(length);

	int *random_indexes = malloc(sizeof(int) * sample_size);

	for (int i = 0; i < sample_size; i++) {
		int rand_i = rand_index(length);
		random_indexes[i] = rand_i;

		// Swap last value with random value
		int tmp = all_indexes[rand_i];
		all_indexes[rand_i] = all_indexes[length - 1];
		all_indexes[length - 1] = tmp;
		
		// Make the last value unavailable
		length--;
	}
	free(all_indexes);

	return random_indexes;
}

/**
* Return an array filled with numbers 1 through length-1.
*	 length: The length of the array to return.
*/
int *fill_array_indexes(int length) {
	int *indexes = malloc(sizeof(int) * length);

	for (int i = 0; i < length; i++) {
		indexes[i] = i;
	}

	return indexes;
}


/**
* Arrange the `n` elements of the array in random order.
* `n` must be must smaller than RAND_MAX to be effective.
*	 array: The array to shuffle.
*		 n: The size of the array.
*/
void shuffle(int *array, int n) {
	start_srand();

	for (int i = n - 1; i > 0; i--) {
		int j = rand() % (i + 1);

		swap(&array[i], &array[j]);
	}
}

/**
* Utility function to swap two integers.
*	a, b: The integers to swap.
*/
void swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}


/**
* Seperate a string by the delimeter.
*	   str: String to modify
*	 delim: The character to seperate the string by.
*/
char *str_sep(char **str, const char* delim) {

	char* start = *str;
	char* p;

	p = (start != NULL) ? strpbrk(start, delim) : NULL;

	if (!p) {
		*str = NULL;
	}
	else {
		*p = '\0';
		*str = p + 1;
	}

	return start;
}


/**
* Remove the spaces from a string.
*	 str: The string to modify.
*/
void remove_spaces(char *str) {
	if (!str) return;

	char* i = str;
	char* j = str;

	while (*j) {
		*i = *j++;
		if (*i != ' ') i++;
	}

	*i = 0;
}

/**
* Print a 2 dimensional array of doubles. Each outer array must end
* with a NULL pointer. Each inner array must end with the NAN value
* defined in <math.h>
	 arr: The 2D array to print.
*/
void print_2d_array(double **arr) {
	while (*arr) {
		while (!isnan(**arr)) {
			printf("%f\n", *(*arr)++);
		}

		arr++;
	}
}

/**
* Return if a string is in an array.
*	 strings: The array of strings to parse.
*	     str: The string to parse for.
*		size: The size of the array of strings.
*/
bool str_in_arr(char **strings, char *str, int size) {
	
	for (int i = 0; i < size; i++) {
		if (!strcmp(strings[i], str)) {
			return true;
		}
	}

	return false;
}

/**
* Return average and standard deviation.
*	 values: Values to calculate on.
*	   size: The number of elements in values.
*/
double *get_ave_and_std(double *values, int size) {
	double ave = sum_doubles(values, size) / size;
	double std = get_std(values, size, ave);

	double *results = malloc(sizeof(double) * 2);
	results[0] = ave;
	results[1] = std;

	return results;
}

/**
* Return the sum of the doubles in an array.
*	 values: The array.
*	   size: The size of the array.
*/
double sum_doubles(double *values, int size) {
	double total = 0;

	for (int i = 0; i < size; i++) {
		total += values[i];
	}

	return total;
}

/**
* Return the standard deviation of a set of values.
*	 values: The array of values.
*	   size: The size of the array.
*		ave: The average of the values.
*/
double get_std(double *values, int size, double ave) {
	double std = 0.0;

	for (int i = 0; i < size; ++i) {
		std += pow(values[i] - ave, 2);
	}

	return sqrt(std / 10);
}

/**
* Get the length of a double, provided the user knows
* the  number of digits after the decimal point to use.
*	 d: The double.
*	 d_places: The number of digits after the decimal point
*/
int double_length(double d, int d_places) {
	int d_int = (int)d;

	if (!d_int) return 1 + d_places;

	int n_digs = (int)floor(log10(abs(d_int))) + 1;

	return n_digs + d_places;
}

/**
* Get the length of an integer.
*	 i: The integer to check.
*/
int int_length(int i) {
	return double_length((double)i, 0);
}

/**
* Get the max value of all doubles in an array.
*	 values: The array to parse.
*	   size: The size of the array.
*/
double max_value(double *values, int size) {
	double max = values[0];

	for (int i = 1; i < size; i++) {
		if (values[i] > max) max = values[i];
	}

	return max;
}

/**
* Add the constants from the header of a csv file to
* a hashmap of arities.
*	 arities: The hashmap to add to.
*/
void add_constants_from_csv(struct hashmap *arities) {
	csv_reader *reader = init_csv(get_fitness_file(), ',');

	csv_line *header = get_header(reader);
	char **line = header->content;
	int size = header->size;

	for (int i = 0; i < size - 1; i++) {
		hashmap_put(arities, line[i], 0);
	}

	deinit_csv(reader);
}

/**
* Return the fitness file. Automatically found by CMake
* If not using CMake, manually input it in this function.
* Fitness file must be in <root>/data
*/
char *get_fitness_file() {
#ifdef CSV_DIR
	return CSV_DIR;
#endif

	fprintf(stderr, "CSV file not found. Please include in the folder <root>/data");
	abort();
}

/**
* Parse a config file. The extension must be .ini
*/
struct hashmap **parse_config() {
	struct hashmap *arities = hashmap_init();
	struct hashmap *params = hashmap_init();

	ini_parser *p = create_ini_parser(get_config_file());

	for (int i = 0; i < p->num_sections; i++) {
		ini_section section = p->sections[i];

		for (int k = 0; k < section.num_pairs; k++) {
			if (!strcmp(section.header, "[arities]")) {
				hashmap_put(arities, section.pairs[k].key, section.pairs[k].values[0]);
			}
			else if (!strcmp(section.header, "[search_parameters]")) {
				hashmap_put(params, section.pairs[k].key, section.pairs[k].values[0]);
			}
			else if (!strcmp(section.header, "[constants]")) {
				for (int x = 0; x < section.pairs->num_values; x++) {
					double d = section.pairs[0].values[x];

					char *str = int_to_string((int)d);

					hashmap_put(arities, str, 0);
				}
			}
		}
	}

	free_parser(p);
	add_constants_from_csv(arities);

	// Verbose printing defaults to zero.
	if (isnan(hashmap_get(params, "verbose"))) hashmap_put(params, "verbose", 0);

	struct hashmap **config = malloc(2 * sizeof(struct hashmap *));
	config[0] = arities;
	config[1] = params;

	return config;
}



/**
* Return the config file. Automatically found by CMake
* If not using CMake, manually input it in this function.
* Fitness file must be in <root>/data. File must have extension
* .ini
*/
char *get_config_file() {
#ifdef INI_DIR
	return INI_DIR;
#endif

	fprintf(stderr, "Config file not found. Please include in the folder <root>/data");
	abort();
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

	double **training_cases = malloc((sizeof(double *) * fits_split_i) + 1);
	double **test_cases = malloc((sizeof(double *) * (fitness_len - fits_split_i)) + 1);

	for (int i = 0; i < 2; i++) {
		training_cases[i] = malloc(sizeof(double) * col_size);
		test_cases[i] = malloc(sizeof(double) * col_size);
	}

	double *training_targets = malloc((sizeof(double) * fits_split_i) + 1);
	double *test_targets = malloc(sizeof(double) * (targs_len - fits_split_i) + 1);

	int rand_i;
	int i;

	for (i = 0; i < fitness_len; i++) {
		rand_i = fits_rand_idxs[i];

		if (i >= fits_split_i) {
			test_cases[i - fits_split_i] = fitness[rand_i];
			test_targets[i - fits_split_i] = targs[rand_i];
		}
		else {
			training_cases[i] = fitness[rand_i];
			training_targets[i] = targs[rand_i];
		}
	}
	// Set last index to NULL/NAN to allow for easier looping of arrays
	training_cases[fits_split_i] = NULL;
	test_cases[fitness_len - fits_split_i] = NULL;
	training_targets[fits_split_i] = NAN;
	test_targets[targs_len - fits_split_i] = NAN;

	struct csv_data *results = malloc(sizeof(struct csv_data));
	results->test_cases = test_cases;
	results->training_cases = training_cases;
	results->training_targets = training_targets;
	results->test_targets = test_targets;

	free(exemplars);
	free(fits_rand_idxs);

	return results;
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
struct symbols *get_symbols(struct hashmap *arities) {
	struct symbols *symbols = malloc(sizeof(struct symbols));
	symbols->arities = arities;

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
