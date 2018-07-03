
#ifndef PONY_GP_PARAMS_H
#define PONY_GP_PARAMS_H

#include <stdbool.h>

extern bool VERBOSE;
extern int POPULATION_SIZE;
extern int MAX_DEPTH;
extern int ELITE_SIZE;
extern int GENERATIONS;
extern int TOURNAMENT_SIZE;
extern double SEED;
extern double CROSSOVER_PROBABILITY;
extern double MUTATION_PROBABILITY;
extern double TEST_TRAIN_SPLIT;

extern char *CONFIG_DIR;
extern char *CSV_DIR;

#endif //PONY_GP_PARAMS_H
