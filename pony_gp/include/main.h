
#ifndef PONY_GP_MAIN_H
#define PONY_GP_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <float.h>
#include <ctype.h>
#include "../include/memmngr.h"
#include "../include/binary_tree.h"
#include "../include/queue.h"
#include "../include/rand_util.h"
#include "../include/misc_util.h"
#include "../include/hashmap.h"
#include "../include/params.h"
#include "../include/config_parser.h"
#include "../include/csv_parser.h"
#include "../include/tests.h"
#include "include/data.h"

#define DEFAULT_FITNESS -DBL_MAX

struct individual {
    struct node *genome;
    double fitness;
};

void subtree_mutation(struct node *root);
void setup(void);
char get_random_symbol(int curr_depth, int max_depth, bool must_fill);
struct node **subtree_crossover(struct node *p1, struct node *p2);
struct individual *new_individual(struct node *genome, double fitness);
void free_individual(struct individual *i);
void print_individual(struct individual *i);
double evaluate(struct node *node, double *fitness_case);
void evaluate_individual(struct individual *ind);
void evaluate_population(struct individual **pop);
void init_population(struct individual **pop);
void sort_population(struct individual **pop, int size);
int fitness_comp(const void *elem1, const void *elem2);
void print_population(struct individual **pop, int size);
void print_stats(int generation, struct individual **pop, double duration);
struct individual **tournament_selection(struct individual **pop);

#endif //PONY_GP_MAIN_H
