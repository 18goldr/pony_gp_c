#include "../include/hashmap.h"

/**
 * Allocate memory for a hashmap. Initialize fields.
 * @return The new hashmap.
 */
struct hashmap *init_hashmap() {
    struct hashmap *h = allocate_m(sizeof(struct hashmap));

    h->num_pairs = 0;

    return h;
}

/**
 * Free the memory allocated for a hashmap and all its key-value pairs.
 * @param h The hashmap to free.
 */
void free_hashmap(struct hashmap *h) {
    for (int i=0; i < h->num_pairs; i++) {
        free_pointer(h->keys[i]);
    }

    free_pointer(h);
}

/**
 * Assign a key-value pair.
 * @param h The hashmap to append to.
 * @param key The key.
 * @param value The value associated with the key.
 * @return If the key-value pair was successfully assigned.
 */
int put_hashmap(struct hashmap *h, char *key, const double value) {
    int i = h->num_pairs;

    if (i > MAX_HASHMAP_SIZE - 1) return EXIT_FAILURE;

    h->keys[i] = key;
    h->values[i] = value;

    h->num_pairs++;

    return EXIT_SUCCESS;
}

/**
 * Get the value assigned to a key. Returns NAN if the key
 * is not found.
 * @param h The hashmap to retrieve the value from.
 * @param key The key to search for.
 * @return The value assigned to the given key.
 */
double get_hashmap(struct hashmap *h, char *key) {
    if (!h->keys[0]) return NAN;

    for (int i=0; i < h->num_pairs; i++) {
        if (!strcmp(h->keys[i], key)) {
            return h->values[i];
        }
    }

    return NAN;
}

/**
 * Print a hashmap's key-value pairs to the console.
 * @param h The hashmap to print.
 */
void print_hashmap(struct hashmap *h) {
    for (int i=0; i < h->num_pairs; i++) {
        printf("%s: %f\n", h->keys[i], h->values[i]);
    }
}
