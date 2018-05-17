#include "../include/rand_util.h"

/**
 * Set the seed to be given to srand.
 * @param s The value that SEED will be set to.
 */
void set_seed(double s) {
    SEED = s;
}
/**
 * A wrapper for srand. Ensures that it is only called once per execution.
 */
void start_srand() {
    // In order for the other random functions to work as expected,
    // this must be called exactly once per execution. This is because
    // srand will return different results only if it is called once.

    static bool called = false;

    if (!called) {
        srand((unsigned int) (SEED ? SEED : time(NULL)));
        called = true;
    }
}

/**
 * Generate a random integer between min and max (inclusive).
 * @param min The minimum value.
 * @param max The maximum value.
 * @return The randomly generated integer.
 */
int get_randint(int min, int max) {
    return (rand() % (max + 1 - min)) + min;
}

/**
 * Generate a random probability. Values are 0.0 to 1.0 (inclusive).
 * @return A randomly generated double.
 */
double get_rand_probability() {
    return (double)rand() / (double)RAND_MAX;
}

/**
 * Return an an of random indexes of `n` length.
 * @param n Length of the array.
 * @return The random indexes.
 */
int *rand_indexes(int n) {
    int *indexes = allocate_m(sizeof(int) * n);

    for (int i=0; i < n; i++) {
        indexes[i] = i;
    }

    shuffle(indexes, n);

    return indexes;
}

/**
 * Randomly rearrange `n` elements of an array.
 * In order to work effectively, `n` must be smaller than RAND_MAX.
 * @param a The array to shuffle.
 * @param n The length of the array.
 */
void shuffle(int *a, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = get_randint(0, i + 1);

        swap(&a[i], &a[j]);
    }
}
