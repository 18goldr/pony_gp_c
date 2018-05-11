#include "../include/file_util.h"

int MAX_LINE_LENGTH = 2048;
char comment_sym = '#';
const char delimeter[2] = ":";
const char const_delimeter[2] = ",";

/**
 * Get the number of lines in a file, ignoring comments and empty lines.
 * @param file The file to parse.
 * @return The number of lines.
 */
int get_num_lines(FILE *file) {
    reset_file_position(file);

    if (!file) return 0;

    int count = 1; // If the file is not empty, it will be at least one line long.
    char c;

    // Flag to see if line is a comment. If so, ignore.
    bool comment = false;

    // If the current character and the previous characters are new lines,
    // then that means the current line is empty.
    char prev_char;

    for (c = getc(file); c != EOF; c = getc(file)) {

        if (c == comment_sym) comment = true;

        if (c == '\n') {
            if (!comment && prev_char != '\n') count++;

            if (comment) comment = false;

        }

        prev_char = c;
    }

    return count;
}

/**
 * Get the next line of a file.
 * @param file The file to parse.
 * @return The next line of the file.
 */
char *get_line(FILE *file) {
    char *line = allocate_m(MAX_LINE_LENGTH);

    fgets(line, MAX_LINE_LENGTH, file);

    // Remove newline character
    line[strcspn(line, "\n")] = '\0';

    // Ignore line if blank
    while (!line[0]) {
        // If end of file, return NULL.
        if (!fgets(line, MAX_LINE_LENGTH, file)) return NULL;

        // Remove newline character
        line[strcspn(line, "\n")] = '\0';
    }

    return line;
}

/**
 * Return an array of strings containing each line of the file.
 * @param file The file to parse.
 * @return The array of lines.
 */
char **get_lines(FILE *file) {
    reset_file_position(file);

    char **lines = allocate_m(sizeof(char *) * (get_num_lines(file) + 1)); // Leave space for NULL.

    reset_file_position(file);

    char *line;

    int i=0;

    while ((line = get_line(file))) {
        if (line[0] != comment_sym) {
            lines[i] = allocate_m(strlen(line));
            lines[i++] = line;
        }
    }

    lines[i] = NULL;

    return lines;
}

/**
 * Reset the file position to the beginning of the file.
 * A wrapper function for fseek. Use this instead of rewind.
 * @param file
 */
void reset_file_position(FILE *file) {
    fseek(file, 0L, SEEK_SET);
}