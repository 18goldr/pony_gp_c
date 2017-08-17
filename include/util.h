#include <stdbool.h>

#define ARR_SIZE(arr) ((int)(sizeof(arr) / sizeof(0[arr])))

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

