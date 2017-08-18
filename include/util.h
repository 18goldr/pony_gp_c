#include <stdbool.h>
#include <stdio.h>

char *double_to_string(double d, char *format, int sig_decimal_places);
char *getline(FILE *file);
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
