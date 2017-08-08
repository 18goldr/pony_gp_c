#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "hashmap.h"

struct hashmap *hashmap_init() {
	struct hashmap *h = malloc(sizeof(struct hashmap));
	h->curr_index = 0;

	return h;
}

void hashmap_deinit(struct hashmap *h) {
	free(h);
}

int hashmap_put(struct hashmap *h, char *key, const double value) {
	int i = h->curr_index;

	if (i > MAX_SIZE - 1) return EXIT_FAILURE;

	h->keys[i] = key;
	h->values[i] = value;

	h->curr_index++;

	return EXIT_SUCCESS;
}

double hashmap_get(struct hashmap *h, char *key) {
	for (int i = 0; i < h->curr_index; i++) {
		if (!strcmp(h->keys[i], key)) {
			return h->values[i];
		}
	}

	return -DBL_MAX;

}

void print_hashmap(struct hashmap *h) {
	for (int i = 0; i < h->curr_index; i++) {
		printf("%s: %f\n", h->keys[i], h->values[i]);
	}
}