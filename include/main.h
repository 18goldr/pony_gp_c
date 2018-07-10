
#ifndef PONY_GP_MAIN_H
#define PONY_GP_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <float.h>
#include <ctype.h>
#include <stdbool.h>
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

#define DEFAULT_FITNESS (-DBL_MAX)
#define EXPERIMENTAL_OUTPUT 0

struct individual {
    struct node *genome;
    double fitness;
};

void setup(void);
struct individual *run(struct individual **pop);
char get_random_symbol(int curr_depth, int max_depth, bool must_fill);
void subtree_mutation(struct node *root);
struct node **subtree_crossover(struct node *p1, struct node *p2);
struct individual *new_individual(struct node *genome, double fitness);
void free_individual(struct individual *i);
void print_individual(struct individual *i);
double evaluate(struct node *node, double *fitness_case);
void evaluate_individual(struct individual *ind, bool test);
void evaluate_population(struct individual **pop);
void init_population(struct individual **pop);
void sort_population(struct individual **pop, int size);
int fitness_comp(const void *elem1, const void *elem2);
void print_population(struct individual **pop, int size);
void print_stats(int generation, struct individual **pop, double duration);
struct individual **tournament_selection(struct individual **pop);
void generational_replacement(struct individual **new_pop, struct individual **old_pop);
struct individual *search_loop(struct individual **pop);
void swap_populations(struct individual ***pop1, struct individual ***pop2);
void out_of_sample_test(struct individual *i);
void print_params_minimal(void);
void print_settings(void);

#endif //PONY_GP_MAIN_H
