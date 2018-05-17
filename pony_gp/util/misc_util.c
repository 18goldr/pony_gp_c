#include "../include/misc_util.h"

/**
 * Check if a symbol is valid based on the symbols
 * defined in the arities hashmap.
 * @param sym The symbol to check.
 * @param arities The hashmap to check.
 * @return Whether or not the symbol is valid.
 */
bool symbol_is_valid(char sym, struct hashmap *arities) {
    char sym_str[] = {sym, '\0'};

    return (!isnan(get_hashmap(arities, sym_str)));
}

/**
 * Remove the spaces from a string.
 * @param str The string to modify.
 */
void remove_spaces(char *str) {
    if (!str) return;

    char* i = str;
    char* j = str;

    // Create a new string with all spaces removed.
    while (*j) {
        *i = *j++;
        if (*i != ' ') i++;
    }

    *i = 0;
}

/**
 * Swap two integers.
 * @param a, b Integers to swap.
 */
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}