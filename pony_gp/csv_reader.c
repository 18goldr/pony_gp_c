#include "csv_reader.h"

#define MAX_LINE_LENGTH 2048

/**
* Return a new csv_reader.
*	 *file_name: Name of the file that the csv_reader will rad.
*	  delimeter: The character that the file will differentiate 
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
* Return a new csv_line.
*	 **content: The content of the line.
*	      size: The size in characters of the line.
*/
csv_line *new_line(char **content, int size) {
	csv_line *line = malloc(sizeof(struct csv_line));

	line->content = content;

	line->size = size;

	return line;
}

/**
* Get the number of lines in a csv file. 
*	 *reader: The csv_reader containing the file to parse.
*/
int get_num_lines(csv_reader *reader) {
	rewind(reader->file);

	char c;
	int line_length = 0;
	int line_num = 0;

	while ((c = getc(reader->file)) != EOF) {
		if (c == '\n' && line_length > 0) {
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
*	 *reader: The csv_reader containing the file to parse.
*/
int get_num_column(csv_reader *reader) {
	rewind(reader->file);
	char line[MAX_LINE_LENGTH];
	fgets(line, MAX_LINE_LENGTH, reader->file);

	if (line[0] == '\0') return 0;

	int count = 1;

	for (int i = 0; i < strlen(line); i++) {
		count += (line[i] == reader->delimiter);
	}

	rewind(reader->file);
	return count;
}

/**
* Return the values contained in a line of the csv file.
* The reader remembers its position in the document throughout
* seperate calls.
*	 *reader: The csv_reader containing the file to parse.
*/
csv_line *readline(csv_reader *reader) {
	char *f_line = calloc(MAX_LINE_LENGTH, sizeof(char));
	fgets(f_line, MAX_LINE_LENGTH, reader->file);

	// Removes newline character from fgets
	f_line[strcspn(f_line, "\n")] = 0;

	if (!f_line[0]) return NULL;

	int curr_size = DEFAULT_CSV_LINE_LENGTH;
	char **values = calloc(curr_size, sizeof(char *));

	char *token;
	int i = 0;

	while ((token = str_sep(&f_line, &reader->delimiter))) {
		if (i >= curr_size - 1) {
			curr_size += DEFAULT_CSV_LINE_LENGTH;
			values = realloc(values, curr_size * sizeof(char *));
		}

		remove_spaces(token);

		values[i++] = token;
	}

	values[i] = NULL;

	csv_line *line = new_line(values, i);

	return line;

}

/**
* Skip the current line the file.
*	 *reader: The csv_reader containing the document.
*/
void next_line(csv_reader *reader) {
	char ignore[MAX_LINE_LENGTH];
	fgets(ignore, sizeof(ignore), reader->file);
}

/**
* Return the header values.
*	*reader: The csv_reader containing the document.
*/
csv_line *get_header(csv_reader *reader) {
	rewind(reader->file);

	csv_line *line = readline(reader);
	rewind(reader->file);

	return line;
}
