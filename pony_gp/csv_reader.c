#include "csv_reader.h"

#define MAX_LINE_LENGTH 2048

csv_reader *init_csv(char *file_name, char delimeter) {
	csv_reader *reader = malloc(sizeof(struct csv_reader));

	reader->file = fopen(file_name, "r");

	reader->delimiter = delimeter;

	assert(reader->file);

	return reader;
}


csv_line *new_line(char **content, int size) {
	csv_line *line = malloc(sizeof(struct csv_line));

	line->content = content;

	line->size = size;

	return line;
}


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

	int n = 0;

	//for (int i = 0; i < 3; i++) {
	//	printf("%s,", line->content[i]);
	//}
	//printf("\n");

	return line;

}


csv_line *get_header(csv_reader *reader) {
	rewind(reader->file);

	return readline(reader);
}
