#include "../include/config_parser.h"

bool VERBOSE;
int POPULATION_SIZE;
int MAX_DEPTH;
int ELITE_SIZE;
int GENERATIONS;
int TOURNAMENT_SIZE;
double SEED;
double CROSSOVER_PROBABILITY;
double MUTATION_PROBABILITY;
double TEST_TRAIN_SPLIT;
char *CONFIG_DIR;
char *CSV_DIR;

/**
 * Parse command line arguments.
 * @param argc The number of arguments.
 * @param argv The list of arguments
 */
void arg_parse(int argc, char *argv[]) {

    // Both the CSV file and the CONFIG file must be provided for the program to run.
    bool csv_def = false;
    bool config_def = false;

    for (int i=1; i < argc; i+=2) {
        if (strstr(argv[i], "-p")) {
            POPULATION_SIZE = (int) atof(argv[i+1]);
        } else if(strstr(argv[i], "-m")) {
            MAX_DEPTH = (int) atof(argv[i+1]);
        } else if(strstr(argv[i], "-e")) {
            ELITE_SIZE = (int) atof(argv[i+1]);
        } else if(strstr(argv[i], "-g")) {
            GENERATIONS = (int) atof(argv[i+1]);
        } else if(strstr(argv[i], "-ts")) {
            TOURNAMENT_SIZE = (int) atof(argv[i+1]);
        } else if(strstr(argv[i], "-s")) {
            SEED = atof(argv[i+1]);
        } else if(strstr(argv[i], "-cp")) {
            CROSSOVER_PROBABILITY = atof(argv[i+1]);
        } else if(strstr(argv[i], "-mp")) {
            MUTATION_PROBABILITY = atof(argv[i+1]);
        } else if(strstr(argv[i], "-tts")) {
            TEST_TRAIN_SPLIT = atof(argv[i+1]);
        } else if(strstr(argv[i], "-v")) {
            VERBOSE = (bool)atof(argv[i+1]);
        } else if(strstr(argv[i], "-config")) {
            CONFIG_DIR = argv[i+1];
            config_def = true;
        } else if(strstr(argv[i], "-fc")) {
            CSV_DIR = argv[i+1];
            csv_def = true;
        }
    }

    if (!csv_def) {
        fprintf(stderr, "The fitness case file was not provided. Aborting.\n");
    }
    if (!config_def) {
        fprintf(stderr, "The config file was not provided. Aborting.\n");
    }
    if (!config_def || !csv_def) abort();
}

/**
 * Parse the config file for arity values, constants and
 * search parameters. Define a hashmap of arities.
 * @param file The file to parse.
 * @param arities The hashmap to define.
 */
void set_params(FILE *file, struct symbols *s) {

    // Allocate memory for symbol fields
    s->arities = init_hashmap();
    s->functions = allocate_m(MAX_HASHMAP_SIZE);
    s->terminals = allocate_m(MAX_HASHMAP_SIZE);

    // Keep track of number of functions and terminals
    int f_i = 0;
    int t_i = 0;

    char **lines = get_lines(file);

    for (int i = 0; i < get_num_lines(file); i++) {

        char *line = lines[i];

        if (!line) continue;

        // The side of the delimeter you are on.
        // false = left, true = right.
        bool side = false;

        if (strstr(line, "arities")) {

            // Keep adding key-value pairs until the program
            // reaches the next section (signified by square brackets).
            while ((line = lines[++i]) && line[0] != '[') {
                remove_spaces(line);
                remove_last_newline(line);

                char *key = NULL;
                char *value = NULL;

                for (char *t = strtok(line, delimeter); t != NULL; t = strtok(NULL, delimeter), side = !side) {
                    if (!side) {
                        key = t;
                    } else {
                        value = t;
                    }
                }

                put_hashmap(s->arities, key, atof(value));
                s->functions[f_i++] = key[0];

            }
        }

        if (strstr(line, "constants")) {

            line = lines[++i];
            remove_spaces(line);
            remove_last_newline(line);

            // Seperate by the colon.
            for (char *t = strtok(line, delimeter); t != NULL; t = strtok(NULL, delimeter), side = !side) {
                if (side) {
                    // Seperate by commas.
                    for (char *n = strtok(t, const_delimeter); n != NULL; n = strtok(NULL, const_delimeter)) {
                        put_hashmap(s->arities, n, 0.0);

                        s->terminals[t_i++] = n[0];
                    }
                }
            }
        }

        if (strstr(line, "search_parameters")) {
            // Seperate by the colon.
            while ((line = lines[++i])) {
                remove_spaces(line);
                remove_last_newline(line);

                for (char *t = strtok(line, delimeter); t != NULL; t = strtok(NULL, delimeter), side = !side) {
                    if (!side) continue;

                    double td = atof(t);

                    if (strstr(line, "population_size") && !POPULATION_SIZE) {
                        POPULATION_SIZE = (int) td;
                    } else if (strstr(line, "max_depth") && !MAX_DEPTH) {
                        MAX_DEPTH = (int) td;
                    } else if (strstr(line, "elite_size") && !ELITE_SIZE) {
                        ELITE_SIZE = (int) td;
                    } else if (strstr(line, "generations") && !GENERATIONS) {
                        GENERATIONS = (int) td;
                    } else if (strstr(line, "tournament_size") && !TOURNAMENT_SIZE) {
                        TOURNAMENT_SIZE = (int) td;
                    } else if (strstr(line, "seed") && !SEED) {
                        SEED = td;
                    } else if (strstr(line, "crossover_probability") && !CROSSOVER_PROBABILITY) {
                        CROSSOVER_PROBABILITY = td;
                    } else if (strstr(line, "mutation_probability") && !MUTATION_PROBABILITY) {
                        MUTATION_PROBABILITY = td;
                    } else if (strstr(line, "test_train_split") && !TEST_TRAIN_SPLIT) {
                        TEST_TRAIN_SPLIT = td;
                    }

                    // Default verbose to false unless defined
                    // in the config file.
                    if (!VERBOSE) {
                        if (strstr(line, "verbose")) {
                            VERBOSE = (bool) td;
                        } else {
                            VERBOSE = false;
                        }
                    }
                }
            }
        }
    }

    s->functions[f_i] = '\0';
    s->terminals[t_i] = '\0';
    s->term_size = t_i;
    s->func_size = f_i;
}

