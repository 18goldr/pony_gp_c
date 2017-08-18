#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#define DEFAULT_CSV_LINE_LENGTH 3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "util.h"

typedef struct csv_reader {
	FILE *file;
	char delimiter;
} csv_reader;

typedef struct csv_line {
	char **content;
	int size;
} csv_line;

csv_reader *init_csv(char *file_name, char delimeter);
csv_line *readline(csv_reader *reader);
csv_line *get_header(csv_reader *reader);
csv_line *new_line(char **content, int size);
void next_line(csv_reader *reader);
int get_num_column(csv_reader *reader);
int get_num_lines(csv_reader *reader);