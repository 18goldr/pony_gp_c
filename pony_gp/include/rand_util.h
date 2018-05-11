
#ifndef PONY_GP_RAND_UTIL_H
#define PONY_GP_RAND_UTIL_H

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "../include/params.h"

void set_seed(double s);
void start_srand(void);
int get_randint(int min, int max);
double get_rand_probability();

#endif //PONY_GP_RAND_UTIL_H
