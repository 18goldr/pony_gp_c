#include <stdio.h>

typedef struct ini_key_value {
	char *key;
	double *values;
} ini_key_value;

typedef struct ini_section {
	// Each section begins with a header with the format [header]
	char *header;
	ini_key_value *pairs;
} ini_section;

typedef struct ini_parser {
	FILE *file;
	ini_section *sections;
} ini_parser;


ini_parser *create_ini_parser(char *file);
ini_section *get_sections(FILE *file);
int len_section(char **lines, int start);
ini_key_value *get_key_values(char **lines, int start);
char **get_lines(FILE *file);
int num_lines(char **lines);
int num_sections(FILE *file);