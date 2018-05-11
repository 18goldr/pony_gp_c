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
    start_srand();

    return (rand() % (max + 1 - min)) + min;
}

/**
 * Generate a random probability. Values are 0.0 to 1.0 (inclusive).
 * @return A randomly generated double.
 */
double get_rand_probability() {
    start_srand();

    return (double)rand() / (double)RAND_MAX;
}