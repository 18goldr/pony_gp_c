#include <stdlib.h>
#include <stdio.h>
#include "hashmap.h"

void hashmap_init(struct hashmap *h) {
	h->curr_index = 0;
}

void hashmap_deinit(struct hashmap *h) {
	free(h);
}

void hashmap_put(struct hashmap *h, char keys, int values) {

	if (h->curr_index > MAX_SIZE - 1) return;

	h->keys[h->curr_index] = keys;
	h->values[h->curr_index] = values;

	h->curr_index++;
}

int hashmap_get(struct hashmap *h, char keys) {

	for (int i = 0; i < h->curr_index; i++) {

		if (h->keys[i] == keys) return h->values[i];
	}

	return INT_MIN;

}

void print_hashmap(struct hashmap *h) {
	for (int i = 0; i < h->curr_index; i++) {
		printf("%c: %d\n", h->keys[i], h->values[i]);
	}
}