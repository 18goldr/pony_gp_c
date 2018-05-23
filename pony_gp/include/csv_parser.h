
#ifndef PONY_GP_CSV_PARSER_H
#define PONY_GP_CSV_PARSER_H

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "../include/file_util.h"
#include "../include/misc_util.h"
#include "../include/csv_data.h"
#include "../include/rand_util.h"

void csv_add_constants(FILE *file, struct symbols *s);
int get_num_columns(FILE *file);
void parse_exemplars(FILE *file);
void set_test_and_train_data(FILE *file);

#endif //PONY_GP_CSV_PARSER_H
