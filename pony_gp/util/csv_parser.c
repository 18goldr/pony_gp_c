
#include "../include/csv_parser.h"

double **fitness_cases;
double *targets;

int fitness_len;
int targets_len;
int num_lines;
int num_columns;

int fitness_split;

double **test_cases;
double *test_targets;
double **training_cases;
double *training_targets;

int training_len;
int test_len;


/**
 * Parse a CSV file for any variables/constants then add them
 * to a symbols instance. Ignore the last column because that is
 * target value/output.
 * @param file The CSV file.
 * @param s The symbols instance.
 */
void csv_add_constants(FILE *file, struct symbols *s) {
    fseek(file, 0L, SEEK_SET);

    char *line = allocate_m(MAX_LINE_LENGTH);
    fgets(line, MAX_LINE_LENGTH, file);

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

    char *line = allocate_m(MAX_LINE_LENGTH);
    fgets(line, MAX_LINE_LENGTH, file);

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
    for (int i = 1; i < num_lines; i++) {
        line = lines[i];
        remove_spaces(line);
        remove_last_newline(line);

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

    fitness_len = f_i;
    targets_len = t_i;

    assert(fitness_len == targets_len);
}

/**
 * Split the exemplars into testing and training data.
 * @param file The exemplar file to parse.
 */
void set_test_and_train_data(FILE *file) {
    parse_exemplars(file);

    fitness_split = (int)floor(fitness_len * TEST_TRAIN_SPLIT);

    // Leave space for NULL/NAN for easier looping
    training_cases = allocate_m(sizeof(double *) * (fitness_split + 1));
    test_cases = allocate_m(sizeof(double *) * (fitness_len - fitness_split + 1));
    training_targets = allocate_m(sizeof(double) * (fitness_split + 1));
    test_targets = allocate_m(sizeof(double) * (fitness_len - fitness_split + 1));

    // Allocate the inner arrays.
    for (int i=0; i < fitness_split; i++) {
        training_cases[i] = allocate_m(sizeof(double) * num_columns);

        if (i >= fitness_len) {
            test_cases[i - fitness_split] = allocate_m(sizeof(double) * num_columns);
        }
    }

    // Randomize index order access.
    // This function call is what causes the error. Specifically,
    // the call to rand() within the function results in the errors.
    int *fit_rand_idxs = rand_indexes(fitness_len);

    int rand_i;

    // Split fitness and target data into training and test cases.
    for (int i=0; i < fitness_len; i++) {
        rand_i = fit_rand_idxs[i];

        if (i >= fitness_split) {
            test_cases[i - fitness_split] = fitness_cases[rand_i];
            test_targets[i - fitness_split] = targets[rand_i];
        } else {
            training_cases[i] = fitness_cases[rand_i];
            training_targets[i] = targets[rand_i];
        }
    }

    training_cases[fitness_split] = NULL;
    test_cases[fitness_len - fitness_split] = NULL;
    training_targets[fitness_split] = NAN;
    test_targets[fitness_len - fitness_split] = NAN;

    training_len = fitness_split;
    test_len = fitness_len - fitness_split;
}