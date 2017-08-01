#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hashmap.h"

void hashmap_init(struct hashmap *h) {
	h->curr_index = 0;
}

void hashmap_deinit(struct hashmap *h) {
	free(h);
}

int hashmap_put(struct hashmap *h, const char key, const int values) {
	int i = h->curr_index;

	if (i > MAX_SIZE - 1) return EXIT_FAILURE;

	h->keys[i] = key;
	h->values[i] = values;

	h->curr_index++;

	return EXIT_SUCCESS;
}

int hashmap_get(struct hashmap *h, const char key) {
	for (int i = 0; i < h->curr_index; i++) {
		if (h->keys[i] == key) {
			return h->values[i];
		}
	}

	return INT_MIN;

}

void print_hashmap(struct hashmap *h) {
	for (int i = 0; i < h->curr_index; i++) {
		printf("%c: %d\n", h->keys[i], h->values[i]);
	}
}