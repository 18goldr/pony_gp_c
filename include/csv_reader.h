#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#define DEFAULT_CSV_LINE_LENGTH 3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "util.h"

/**
* A struct to hold the necessary components to parse an csv file.
*	 file: The csv file.
*	 delimeter: The character seperating columns.
*/
typedef struct csv_reader {
	FILE *file;
	char delimiter;
} csv_reader;

/**
* A container to hold a lines content and its size.
* The lines content is not just a string of the line,
* it is a pointer to an array of all the values of each
* column in the line. The content does not include 
* delimeters, spaces or anything else that is not
* strictly data.
*	 content: The data contained in the line.
*	    size:
*/
typedef struct csv_line {
	char **content;
	int size;
} csv_line;

void deinit_csv(csv_reader *r);
csv_reader *init_csv(char *file_name, char delimeter);
csv_line *readline(csv_reader *reader);
csv_line *get_header(csv_reader *reader);
csv_line *new_line(char **content, int size);
void next_line(csv_reader *reader);
int get_num_column(csv_reader *reader);
int get_num_lines(csv_reader *reader);