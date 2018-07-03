#include "../include/file_util.h"

char comment_sym = '#';
const char delimeter[2] = ":";
const char const_delimeter[2] = ",";

/**
 * Get the number of lines in a file, ignoring comments and empty lines.
 * Counts the number of new lines.
 * @param file The file to parse.
 * @return The number of lines.
 */
int get_num_lines(FILE *file) {
    reset_file_position(file);

    if (!file) return 0;

    int count = 0;
    char c;

    // Flag to see if line is a comment. If so, ignore.
    bool comment = false;

    // If the current character and the previous characters are new lines,
    // then that means the current line is empty.
    char prev_char = {0};

    for (c = (char) getc(file); c != EOF; c = (char) getc(file)) {

        if (c == comment_sym) comment = true;

        if (c == '\n') {
            if (!comment && prev_char != '\n') {
                count++;
            }

            if (comment) comment = false;

        }

        prev_char = c;
    }

    // Files without trailing empty lines do not end in '\n'.
    if (prev_char != '\n') count++;

    return count;
}


/**
 * Return an array of strings containing each line of the file.
 * @param file The file to parse.
 * @return The array of lines.
 */
char **get_lines(FILE *file) {

    char **lines = allocate_m(sizeof(char *) * (get_num_lines(file) + 1)); // Leave space for NULL

    reset_file_position(file);

    char line[MAX_LINE_LENGTH];

    int i = 0;

    while (fgets(line, MAX_LINE_LENGTH, file)) {

        // Ignore leading spaces.
        unsigned int n;
        for (n = 0; n < strlen(line); n++) {
            if (line[n] != ' ') break;
        }

        if (line[n] != comment_sym && strcmp(line, "\n") != 0) {

            // Leave space for null terminating character
            lines[i] = allocate_m(strlen(line) + 1);
            strncpy(lines[i++], line, strlen(line) + 1);

        }
    }

    lines[i] = NULL;

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