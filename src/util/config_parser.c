#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "config_parser.h"
#include "util.h"

#define DEFAULT_FILE_LENGTH 20
#define MAX_LINE_LENGTH 2048

ini_parser *create_ini_parser(char *file) {
	ini_parser *p = malloc(sizeof(struct ini_parser));

	p->file = fopen(file, "r");
	p->sections = get_sections(p->file);

	return p;
}

ini_section *create_ini_section(char *header, ini_key_value *pairs) {
	ini_section *s = malloc(sizeof(struct ini_section));
	s->header = header;
	s->pairs = pairs;
	return s;
}

ini_key_value *create_ini_key_value(char *key, double *values) {
	ini_key_value *kv = malloc(sizeof(struct ini_key_value));

	kv->key = key;
	kv->values = values;

	return kv;
}

ini_section *get_sections(FILE *file) {
	char **lines = get_lines(file);
	int n = num_lines(lines);

	ini_section *sections = malloc(sizeof(struct ini_section) * num_sections(file));
	int sec_i = 0;
	int i = 0;
	while (i < n) {
		int len = len_section(lines, i);
		char *header = lines[0];
		ini_key_value *pairs = get_key_values(lines, i);

		ini_section *section = create_ini_section(header, pairs);
		sections[sec_i++] = *section;
		i += len;
	}

	return sections;
}

ini_key_value *get_key_values(char **lines, int start) {
	int len = len_section(lines, start) - 1;

	start++;
	const char colon[2] = { ':', '\0' };
	const char comma[2] = { ',', '\0' };

	ini_key_value *pairs = malloc(sizeof(struct ini_key_value) * len);

	for (int i = start; i < (start + len); i++) {
		// Seperate the string
		char *key = str_sep(&lines[i], colon);
		char *values_str = str_sep(&lines[i], colon);

		// Get all values seperated by commas
		int size = 2;
		double *values = calloc(size, sizeof(double));
		char *token;

		if (strchr(values_str, ',')) {
			while ((token = str_sep(&values_str, comma))) {
				values[size - 2] = atof(token);

				values = realloc(values, sizeof(double) * size++);
			}
			values[size - 2] = NAN;
		}
		else {
			values[0] = atof(values_str);
			values[1] = NAN;
		}

		ini_key_value pair = *create_ini_key_value(key, values);

		pairs[i - start] = pair;
	}

	return pairs;
}

/**
* Return the length of an ini_section.
*	lines: All the lines in the file.
*	start: The index of the start of the section in lines;
*/
int len_section(char **lines, int start) {
	if (lines[start][0] != '[') return 0;

	int count = 1;

	while (lines[++start] && lines[start][0] != '[') {
		count++;
	}

	return count;
}




char **get_lines(FILE *file) {
	rewind(file);
	char **doc = malloc(sizeof(char *) * DEFAULT_FILE_LENGTH);
	char *line;
	int i = 0;

	while ((line = getline(file))) {
		remove_spaces(line);
		if (line[0] != '#') {
			doc[i] = malloc(strlen(line));
			doc[i++] = line;

			if (!(DEFAULT_FILE_LENGTH - 1 % i))
				doc = realloc(doc, sizeof(doc) + sizeof(char *) * 10);
		}
	}

	doc[i] = NULL;

	rewind(file);
	return doc;
}


int num_lines(char **lines) {
	int count = 0;

	while (*lines++) {
		count++;
	}

	return count;
}

int num_sections(FILE *file) {
	rewind(file);
	int count = 0;

	char **lines = get_lines(file);

	while (*lines) {
		if (*lines[0] == '[') count++;
		lines++;
	}

	return count;
}