
#ifndef PONY_GP_MISC_UTIL_H
#define PONY_GP_MISC_UTIL_H

#include <stdbool.h>
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
void swap(int *a, int *b);

#endif //PONY_GP_MISC_UTIL_H
