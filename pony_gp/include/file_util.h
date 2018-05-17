
#ifndef PONY_GP_FILE_UTIL_H
#define PONY_GP_FILE_UTIL_H

#include <stdio.h>
#include <string.h>
#include "../include/memmngr.h"

extern int MAX_LINE_LENGTH;
extern char comment_sym;
extern const char delimeter[2];
extern const char const_delimeter[2];

char *get_line(FILE *file);
char **get_lines(FILE *file);
int get_num_lines(FILE *file);
void reset_file_position(FILE *file);
void rmnl(char *line);

#endif //PONY_GP_FILE_UTIL_H
