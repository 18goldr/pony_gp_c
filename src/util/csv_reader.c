#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <assert.h>
#include "util.h"
#include <stdio.h>
#include <string.h>
#include "csv_reader.h"

#define MAX_LINE_LENGTH 2048

/**
* Return a new csv_reader.
*	 file_name: Name of the file that the csv_reader will rad.
*	 delimeter: The character that the file will differentiate
*				 collumns with.
*/
csv_reader *init_csv(char *file_name, char delimeter) {
	csv_reader *reader = malloc(sizeof(struct csv_reader));

	reader->file = fopen(file_name, "r");
	reader->delimiter = delimeter;
	assert(reader->file);

	return reader;
}

/**
* Deallocate space created for the csv_reader.
*	 r: The csv_reader.
*/
void deinit_csv(csv_reader *r) {
	fclose(r->file);

	free(r);
}

/**
* Return a new csv_line.
*	 content: The content of the line.
*	    size: The size in characters of the line.
*/
csv_line *new_line(char **content, int size) {
	csv_line *line = malloc(sizeof(struct csv_line));

	line->content = content;

	line->size = size;

	return line;
}

/**
* Get the number of lines in a csv file.
*	 reader: The csv_reader containing the file to parse.
*/
int get_num_lines(csv_reader *reader) {
	rewind(reader->file);

    if (getc(reader->file) == EOF) return 0;

	char c;
	int line_length = 0;
	int line_num = 1;

	while ((c = getc(reader->file)) != EOF) {
		if (c == '\n' && line_length > 0) { // ignore empty lines
			line_num++;
			line_length = 0;
		}

		line_length++;
	}

	rewind(reader->file);

	return line_num;
}

/**
* Get the number of collumns in the csv file. This function assumes
* that the number of collumns is the same throughout the entire
* document.
*	 reader: The csv_reader containing the file to parse.
*/
int get_num_column(csv_reader *reader) {
	rewind(reader->file);

	char line[MAX_LINE_LENGTH];
	fgets(line, MAX_LINE_LENGTH, reader->file);

	if (line[0] == '\0') return 0;

	int count = 1;

	for (int i = 0; i < (int)strlen(line); i++) {
		count += (line[i] == reader->delimiter);
	}

	rewind(reader->file);
	return count;
}

/**
* Return the values contained in a line of the csv file.
* The reader remembers its position in the document throughout
* seperate calls.
*	 reader: The csv_reader containing the file to parse.
*/
csv_line *readline(csv_reader *reader) {

	// Get the line as a string.
	char *line = calloc(MAX_LINE_LENGTH, sizeof(char));
	fgets(line, MAX_LINE_LENGTH, reader->file);

	if (!line[0]) return NULL;

	// Remove all new line escapes.
	line[strcspn(line, "\n")] = 0;

    char delimeter[] = {reader->delimiter, '\0'};
	int curr_size = DEFAULT_CSV_LINE_LENGTH;

	char **values = calloc(curr_size, sizeof(char *));

	int i = 0;

	// Seperate the line by the given delimeter.
    for (char *t = strtok(line, delimeter); t != NULL; t = strtok(NULL, delimeter)) {
		if (i >= curr_size - 1) {
			curr_size += DEFAULT_CSV_LINE_LENGTH;
			values = realloc(values, curr_size * sizeof(char *));
		}

        char *token = strdup(t);

        remove_spaces(token);

        values[i++] = token;
    }

    free(line);

    values[i] = NULL;

	csv_line *c_line = new_line(values, i);

    return c_line;

}

/**
* Skip the current line of the file.
*	 reader: The csv_reader containing the document.
*/
void next_line(csv_reader *reader) {
	char ignore[MAX_LINE_LENGTH];
	fgets(ignore, sizeof(ignore), reader->file);
}

/**
* Return the header values.
*	reader: The csv_reader containing the document.
*/
csv_line *get_header(csv_reader *reader) {
	rewind(reader->file);

	csv_line *line = readline(reader);
	rewind(reader->file);

	return line;
}
