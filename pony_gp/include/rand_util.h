
#ifndef PONY_GP_RAND_UTIL_H
#define PONY_GP_RAND_UTIL_H

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "../include/params.h"
#include "../include/memmngr.h"
#include "../include/misc_util.h"

void set_seed(double s);
void start_srand(void);
int get_randint(int min, int max);
double get_rand_probability();
int *rand_indexes(int n);
void shuffle(int *a, int n);

#endif //PONY_GP_RAND_UTIL_H
