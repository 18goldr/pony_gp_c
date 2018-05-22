#include "../include/file_util.h"

int MAX_LINE_LENGTH = 4096;
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
 * Return an array of strings containing each line of the file.
 * @param file The file to parse.
 * @return The array of lines.
 */
char **get_lines(FILE *file) {

    char **lines = allocate_m(sizeof(char *) * get_num_lines(file));

    reset_file_position(file);

    char line[MAX_LINE_LENGTH];

    int i = 0;

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        if (line[0] != comment_sym && strcmp(line, "\n")) {

            // Leave space for null terminating character
            lines[i] = allocate_m(strlen(line) + 1);
            strncpy(lines[i++], line, strlen(line) + 1);

        }
    }

    return lines;
}


/**
 * Reset the file position to the beginning of the file.
 * A wrapper function for fseek. Use this instead of rewind.
 * @param file The file pointer.
 */
void reset_file_position(FILE *file) {
    fseek(file, 0L, SEEK_SET);
}