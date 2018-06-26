
#ifndef PONY_GP_MISC_UTIL_H
#define PONY_GP_MISC_UTIL_H

#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "../include/hashmap.h"

/**
 * A wrapper for the functions and terminals that the program can use.
 * Keeps track of arities and the amount of functions and terminals available
 * to the program.
 * @field terminals An array of constants (single digit integers represented as characters).
 * @field functions An array of functions (single characters).
 * @field term_size The amount of constants available.
 * @field func_size The amount of functions available.
 * @field arities A hashmap containing the arities of the different functions.
 */
struct symbols {
    char *terminals;
    char *functions;
    int term_size;
    int func_size;
    struct hashmap *arities;
};

bool symbol_is_valid(char sym, struct hashmap *arities);
void remove_spaces(char *str);
void remove_last_newline(char *str);
void swap(int *a, int *b);
double sum_doubles(double *values, int size);
double get_std(double *values, int size, double ave);
double *get_ave_and_std(double *values, int size);
double max_value(double *values, int size);
double get_time(void);
char *concat(char *s1, char *s2);

#endif //PONY_GP_MISC_UTIL_H
