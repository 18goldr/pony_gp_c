#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "hashmap.h"
#include <math.h>

/**
* Return a new hashmap.
*/
struct hashmap *hashmap_init() {
	struct hashmap *h = malloc(sizeof(struct hashmap));
	h->curr_index = 0;

	return h;
}

/**
* Deallocate the hashmap. Must be called in order to avoid
* memory leaks.
*	 *h: The hashmap to deallocate.
*/
void hashmap_deinit(struct hashmap *h) {
	free(h);
}

/**
* Assign a key-value pair.
*	    *h: The hashmap to assign it to.
*	  *key: The key.
*	 value: The value.
*/
int hashmap_put(struct hashmap *h, char *key, const double value) {
	int i = h->curr_index;

	if (i > MAX_SIZE - 1) return EXIT_FAILURE;

	h->keys[i] = key;
	h->values[i] = value;

	h->curr_index++;

	return EXIT_SUCCESS;
}

/**
* Get the value of the given key. Returns NAN if not found.
* 	   *h: The hashmap to retrieve the value from.
*	 *key: The key to search for.
*/
double hashmap_get(struct hashmap *h, char *key) {
	for (int i = 0; i < h->curr_index; i++) {
		if (!strcmp(h->keys[i], key)) {
			return h->values[i];
		}
	}

	return NAN;

}

/**
* Print a hashmap to the console.
*	 *h: The hashmap to print.
*/
void print_hashmap(struct hashmap *h) {
	for (int i = 0; i < h->curr_index; i++) {
		printf("%s: %f\n", h->keys[i], h->values[i]);
	}
}