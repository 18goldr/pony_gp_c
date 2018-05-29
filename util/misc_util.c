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

void remove_last_newline(char *str) {
    str[strcspn(str, "\n")] = '\0';
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

/**
 * Return the sum of the double in an array.
 * @param values The array.
 * @param size The size of the array.
 * @return The sum.
 */
double sum_doubles(double *values, int size) {
    double total = 0;

    for (int i=0; i < size; i++) {
        total += values[i];
    }

    return total;
}

/**
 * Return the standard deviation of an array of values.
 * @param values The array of values.
 * @param size The size of the array.
 * @param ave The average of the values in the array.
 * @return The standard deviation.
 */
double get_std(double *values, int size, double ave) {
    double std = 0;

    for (int i=0; i < size; i++) {
        std += pow(values[i] - ave, 2);
    }

    return sqrt(std / 10);
}

/**
 * The the average and standard deviation of an array of values.
 * The first element of the array returned is the average and
 * the second is the standard devitation.
 * @param values The array of values.
 * @param size The size of the array
 * @return The average and standard deviation.
 */
double *get_ave_and_std(double *values, int size) {
    double ave = sum_doubles(values, size) / size;
    double std = get_std(values, size, ave);

    double *results = allocate_m(sizeof(double) * 2);

    results[0] = ave;
    results[1] = std;

    return results;
}

/**
 * Return the max value in a double array.
 * @param values The array to parse.
 * @param size The size of the array.
 * @return The max value.
 */
double max_value(double *values, int size) {
    double max = values[0];

    for (int i=1; i < size; i++) {
        if (values[i] > max) max = values[i];
    }

    return max;
}

/**
 * Get the current time in nanoseconds.
 * @return The current time in nanoseconds.
 */
double get_time() {
    struct timespec t;
    timespec_get(&t, TIME_UTC);
    return (double)(t.tv_sec * 1000000000L + t.tv_nsec) / 1000000000L;
}