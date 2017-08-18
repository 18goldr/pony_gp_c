#include <stdio.h>

/**
* A key-value(s) pair struct. Basically a single
* entry of a hashmap.
*	       key: The key.
*	    values: The values.
*	num_values: The number of values the key points to.
*/
typedef struct ini_pair {
	char *key;
	double *values;
	int num_values;
} ini_pair;

/**
* A container to represent a section of an ini file.
* A section is a part of an ini file. The header of the 
* section is indicated by []. Inside the section is a variable
* amount of key value pairs (represented as an ini_pair pointer
* to an array). Keys can point to one or more values. The values
* must be numbers.
* Example section:
*
*	 [header]
*		 key: value
*		key1: value1 
*	    keys: value2, value3, value4
*
*/
typedef struct ini_section {
	char *header;
	ini_pair *pairs;
	int num_pairs;
} ini_section;

/**
* A struct to hold the necessary components to parse an ini file.
*	         file: The file to parse.
*	     sections: A pointer to an array of ini_sections 
*			       used to represent the sections in the file.
*	 num_sections: The number of sections in the file.
*/
typedef struct ini_parser {
	FILE *file;
	ini_section *sections;
	int num_sections;
} ini_parser;


ini_parser *create_ini_parser(char *file);
ini_section *get_sections(FILE *file);
int len_section(char **lines, int start);
ini_pair *get_pairs(char **lines, int start);
char **get_lines(FILE *file);
int num_lines(char **lines);
void free_parser(ini_parser *p);