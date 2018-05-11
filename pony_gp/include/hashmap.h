
#ifndef PONY_GP_HASHMAP_H
#define PONY_GP_HASHMAP_H

#define MAX_HASHMAP_SIZE 100

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "../include/memmngr.h"

/**
 * A simple implementation of a hashmap. The structure
 * contains two arrays, which together hold key-value pairs.
 * @field keys The array of keys
 * @field values The array of values
 * @field num_pairs The number of key-value pairs currently assigned.
 */
struct hashmap {
    char *keys[MAX_HASHMAP_SIZE];
    double values[MAX_HASHMAP_SIZE];
    int num_pairs;
};

struct hashmap *init_hashmap(void);
void free_hashmap(struct hashmap *h);
int put_hashmap(struct hashmap *h, char *key, const double value);
double get_hashmap(struct hashmap *h, char *key);
void print_hashmap(struct hashmap *h);

#endif //PONY_GP_HASHMAP_H
