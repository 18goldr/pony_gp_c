#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define ARR_SIZE(arr) ((int)(sizeof(arr) / sizeof(0[arr])))

int get_char_arr_length(char *arr);
int get_randint(int min, int max);
double get_rand_probability(void);
int rand_index(int length);
void start_srand(void);
bool char_in(char *arr, char c);