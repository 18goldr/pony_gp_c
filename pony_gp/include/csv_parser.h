
#ifndef PONY_GP_CSV_PARSER_H
#define PONY_GP_CSV_PARSER_H

#include <stdio.h>
#include "../include/file_util.h"
#include "../include/misc_util.h"
#include "../include/csv_data.h"

void csv_add_constants(FILE *file, struct symbols *s);
int get_num_columns(FILE *file);
void parse_exemplars(FILE *file);

#endif //PONY_GP_CSV_PARSER_H
