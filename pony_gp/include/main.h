
#ifndef PONY_GP_MAIN_H
#define PONY_GP_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include "../include/memmngr.h"
#include "../include/binary_tree.h"
#include "../include/queue.h"
#include "../include/rand_util.h"
#include "../include/misc_util.h"
#include "../include/hashmap.h"
#include "../include/params.h"
#include "../include/config_parser.h"
#include "../include/csv_parser.h"
#include "include/data.h"

struct individual {
    struct node *genome;
    double fitness;
};

void subtree_mutation(struct node *root);
void run_tests(void);
void get_node_at_index_test(void);
void get_max_tree_depth_test(void);
void setup(void);
char get_random_symbol(int curr_depth, int max_depth, bool must_fill);
struct node **subtree_crossover(struct node *p1, struct node *p2);
void subtree_mutation_test(void);
void subtree_crossover_test(void);
struct individual *new_individual(struct node *genome, double fitness);
void free_individual(struct individual *i);
void print_individual(struct individual *i);

#endif //PONY_GP_MAIN_H
