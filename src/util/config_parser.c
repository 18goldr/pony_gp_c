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

static int get_num_sections(FILE *file);

/**
* Return a new ini_parser.
*	 file: The file to parse.
*/
ini_parser *create_ini_parser(char *file) {
	ini_parser *p = malloc(sizeof(struct ini_parser));

	p->file = fopen(file, "r");
	p->sections = get_sections(p->file);
	p->num_sections = get_num_sections(p->file);

	return p;
}

/**
* Deallocate a parser.
*	p: The parser to deallocate.
*/
void free_parser(ini_parser *p) {
	for (int i = 0; i < p->num_sections; i++) {
		free(p->sections[i].pairs);
	}

	fclose(p->file);
	free(p->sections);
	free(p);
}

/**
* Return a new ini_section.
*	    header: The header of the section with format [content]
*	     pairs: The pairs contained in the section.
*	 num_pairs: The number of pairs in the section.
*/
ini_section *create_ini_section(char *header, ini_pair *pairs, int num_pairs) {
	ini_section *s = malloc(sizeof(struct ini_section));
	s->header = header;
	s->pairs = pairs;
	s->num_pairs = num_pairs;

	return s;
}

/**
* Return a new ini_pair.
*	        key: The key of the pair.
*	     values: The values of the key.
*	 num_values: The number of values for the key.
*/
ini_pair *create_ini_pair(char *key, double *values, int num_values) {
	ini_pair *kv = malloc(sizeof(struct ini_pair));

	kv->key = key;
	kv->values = values;
	kv->num_values = num_values;

	return kv;
}

/**
* Return the sections in a file.
*	 file: The file to parse.
*/
ini_section *get_sections(FILE *file) {
	char **lines = get_lines(file);
	int n = num_lines(lines);

	ini_section *sections = malloc(sizeof(struct ini_section) * get_num_sections(file));
	int sec_i = 0;
	int i = 0;
	
	while (i < n) {
		int len = len_section(lines, i);
		char *header = lines[i];
		ini_pair *pairs = get_pairs(lines, i);

		ini_section *section = create_ini_section(header, pairs, len-1);
		sections[sec_i++] = *section;
		i += len;
		free(section);
	}

	return sections;
}

/**
* Return the ini_pairs in a file.
*	 lines: The lines of the file seperated in an array of strings.
*	 start: The line number to start on.
*/
ini_pair *get_pairs(char **lines, int start) {
	int len = len_section(lines, start) - 1;

	start++;
	const char colon[2] = { ':', '\0' };
	const char comma[2] = { ',', '\0' };

	ini_pair *pairs = malloc(sizeof(struct ini_pair) * len);

	for (int i = start; i < (start + len); i++) {
		// Seperate the string
		char *key = str_sep(&lines[i], colon);
		char *values_str = str_sep(&lines[i], colon);

		// Get all values seperated by commas
		double *values;
		char *token;
		int size;

		if (strchr(values_str, ',')) {
            size = 1;
            values = malloc(sizeof(double));

			while ((token = str_sep(&values_str, comma))) {
                values[size - 1] = atof(token);
				values = realloc(values, sizeof(double) * ++size);
			}

			values[size - 1] = NAN;
		}
		else {
            size = 2;
            values = malloc(sizeof(double) * size);
			values[0] = atof(values_str);
			values[1] = NAN;
		}

		ini_pair *pair = create_ini_pair(key, values, size - 1);

		pairs[i - start] = *pair;
		free(pair);
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

/**
* Return the lines in a file.
*	 file: The file to parse.
*/
char **get_lines(FILE *file) {
	rewind(file);
	char **doc = malloc(sizeof(char *) * DEFAULT_FILE_LENGTH);
	char *line;
	int i = 0;

	while ((line = get_line(file))) {
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

/**
* Return the number of lines in a file.
*	 lines: The lines of the file seperated in a string array.
*/
int num_lines(char **lines) {
	int count = 0;

	while (*lines++) {
		count++;
	}

	return count;
}

/**
* Get the number of sections in a file.
* Should not be called except before or during the constructor.
* To get number of sections elsewhere, use the integer member 
* "num_selections".
*	 file: The file to parse.
*/
static int get_num_sections(FILE *file) {
	rewind(file);
	int count = 0;

	char **lines = get_lines(file);

	while (*lines) {
		if (*lines[0] == '[') count++;
		lines++;
	}

	return count;
}

