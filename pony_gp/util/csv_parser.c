
#include "../include/csv_parser.h"

double **fitness_cases;
double *targets;

/**
 * Parse a CSV file for any variables/constants then add them
 * to a symbols instance. Ignore the last column because that is
 * target value/output.
 * @param file The CSV file.
 * @param s The symbols instance.
 */
void csv_add_constants(FILE *file, struct symbols *s) {
    fseek(file, 0L, SEEK_SET);

    char *line = get_line(file);
    remove_spaces(line);

    int t_i = s->term_size;
    int c = get_num_columns(file) - 1; // Ignore the last column because that is an output.
    int i = 0;

    for (char *t = strtok(line, const_delimeter); t != NULL, i < c; t = strtok(NULL, const_delimeter), i++) {
        s->terminals[t_i++] = t[0];

        put_hashmap(s->arities, t, 0.0);
    }

    s->term_size = t_i;
}

/**
 * Get the number of columns in a CSV file.
 * @param file The CSV file to parse.
 * @return The number of columns.
 */
int get_num_columns(FILE *file) {
    reset_file_position(file);

    char *line = get_line(file);
    int count = 0;

    for (char *t = strtok(line, const_delimeter); t != NULL; t = strtok(NULL, const_delimeter)) {
        count++;
    }

    return count;

}

/**
 * Parse a CSV file for fitness cases and target values (inputs and outputs).
 * In each row of the CSV, the last column is the target value of the exemplar.
 * @param file The CSV file to parse.
 */
void parse_exemplars(FILE *file) {
    reset_file_position(file);

    char **lines = get_lines(file);

    int num_lines = get_num_lines(file);
    int num_columns = get_num_columns(file);

    // Allocate fitness_cases.
    // Leave space for NULL
    fitness_cases = allocate_m(sizeof(double *) * num_lines);
    targets = allocate_m(sizeof(double) * num_lines);

    int t_i = 0; // Index counter for targets
    int f_i = 0; // Index counter for fitness cases

    char *line;
    // Ignore the header.
    for (int i=1; i < num_lines; i++) {
        line = lines[i];
        remove_spaces(line);

        // Current column
        int c = 0;

        double *inputs = allocate_m(sizeof(double) * num_columns); // Leave space for NAN.

        for (char *t = strtok(line, const_delimeter); t != NULL; t = strtok(NULL, const_delimeter), c++) {

            if (c == num_columns - 1) {
                // The last column will always contain a target/desired output.
                targets[t_i++] = atof(t);
            } else {
                inputs[c] = atof(t);
            }
        }

        inputs[c-1] = NAN;

        fitness_cases[f_i++] = inputs;
    }

    fitness_cases[f_i] = NULL;
    targets[t_i] = NAN;
}