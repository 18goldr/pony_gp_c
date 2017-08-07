#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define ARR_SIZE(arr) ((int)(sizeof(arr) / sizeof(0[arr])))

int get_char_arr_length(char *arr);
int get_randint(int min, int max);
double get_rand_probability(void);
int rand_index(int length);
void start_srand(void);
void set_seed(double seed);
int *random_indexes(int length);
void shuffle(int *array, int n);
char *str_sep(char **stringp, const char *delim);
void remove_spaces(char *str);

