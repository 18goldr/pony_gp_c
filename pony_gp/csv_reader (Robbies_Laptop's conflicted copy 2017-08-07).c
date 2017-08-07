#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct csv_reader {
	char *file_name;
	char *delimeter;
	bool has_header;
	FILE *file;
} csv_reader;

csv_reader *new_csv_reader(char *f_name, bool header) {
	csv_reader *csv = malloc(sizeof(csv_reader));
	if (!f_name) {
		return NULL;
	} else {
		csv->file_name = f_name;
	}

	csv->has_header = header;
	csv->delimeter = ",";
	
	return csv;
}