#ifndef HASHMAP_H
#define HASHMAP_H

#define MAX_SIZE 100

/**
* A simple implementation of a hashmap consisting of
* two arrays, which together hold a key value pair.
*		  keys: The array of keys
*		values: The array of the value.
*	curr_index: Current max index of keys and values
*/
struct hashmap {
	char *keys[MAX_SIZE];
	double values[MAX_SIZE];
	int curr_index; 
};

int hashmap_put(struct hashmap *h, char *key, const double value);
double hashmap_get(struct hashmap *h, char *key);
struct hashmap *hashmap_init(void);
void hashmap_deinit(struct hashmap *h);
void print_hashmap(struct hashmap *h);
int hashmap_get_size(struct hashmap *h);
char *hashmap_get_key(struct hashmap *h, int i);
int hashmap_get_num_with_value(struct hashmap *h, int value);

#endif