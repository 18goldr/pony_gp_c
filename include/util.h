#ifndef UTIL_H
#define UTIL_H

#define MAX_LINE_LENGTH 2048
#define MAX_STRING_LENGTH 2048

#include <stdbool.h>
#include <stdio.h>

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

char *double_to_string(double d, char *format, int sig_decimal_places);
char *get_line(FILE *file);
int get_char_arr_length(char *arr);
int get_double_arr_length(double *arr);
int get_randint(int min, int max);
double get_rand_probability(void);
int rand_index(int length);
void start_srand(void);
void set_seed(double seed);
int *random_indexes(int length);
void shuffle(int *array, int n);
char *str_sep(char **stringp, const char *delim);
void remove_spaces(char *str);
int get_2d_arr_length(double **arr);
void print_2d_array(double **arr);
bool str_in_arr(char **strings, char *str, int size);
int *fill_array_indexes(int length);
int *random_sample(int length, int sample_size);
double *get_ave_and_std(double *values, int size);
double sum_doubles(double *values, int size);
double get_std(double *values, int size, double ave);
double max_value(double *values, int size);
void swap(int *a, int *b);
int double_length(double d, int d_places);
int int_length(int i);
char *int_to_string(int i);
void add_constants_from_csv(struct hashmap *params);
char *get_fitness_file(void);
struct hashmap **parse_config(void);
char *get_config_file(void);
struct exemplars *parse_exemplars(char *file_name);
struct csv_data *get_test_and_train_data(char *file_name, double split);
bool symbol_is_valid(char sym, struct hashmap *arities);
struct symbols *get_symbols(struct hashmap *arities);

#endif
