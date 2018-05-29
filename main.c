#include "include/main.h"

// The list of symbols the program uses to generate individuals.
struct symbols *symbols;

// Cache for fitness evaluation.
struct hashmap *pop_cache;

int main(int argc, char *argv[]) {
    if (argc) arg_parse(argc, argv);

    init_memory(DEFAULT_MEMORY_POOL_SIZE);

    setup();

    struct individual **population = allocate_m(sizeof(struct individual *) * POPULATION_SIZE);
    struct individual *best_ever = run(population);

    printf("\nBest solution on the training data: ");
    print_individual(best_ever);
    printf("\n");
    out_of_sample_test(best_ever);

    destroy_memory();

    exit(EXIT_SUCCESS);
}

/**
 * Return the best solution. Initialize a population.
 * Perform an evolutionary search.
 * @param pop The population to initialize.
 * @return The best individual solution.
 */
struct individual *run(struct individual **pop) {
    init_population(pop);

    struct individual *best_ever = search_loop(pop);

    return best_ever;
}

/**
 * Set up memory pool, set seed, define symbols.
 */
void setup() {
    // Define symbols
    symbols = allocate_m(sizeof(struct symbols));

    pop_cache = init_hashmap();

    FILE *config = fopen(CONFIG_DIR, "r");
    set_params(config, symbols);
    fclose(config);

    start_srand();

    FILE *csv = fopen(CSV_DIR, "r");
    csv_add_constants(csv, symbols);
    set_test_and_train_data(csv);
    fclose(csv);
}

/**
 * Return a randomly chosen symbol (function or terminal). The current depth
 * determines whether a terminal or function should be chosen. If `full` is true,
 * a function will be returned unless the max depth has been reached. The choices within
 * each symbol type (terminal or function) are selected with a uniform probability.
 * @param curr_depth The current depth in the binary tree.
 * @param max_depth The max depth the tree is allowed to reach.
 * @param must_fill A determinant for whether functions are among the pool of possible symbols.
 *                    If true, the symbol may be a function.
 * @return The randomly chosen symbol.
 */
char get_random_symbol(int curr_depth, int max_depth, bool must_fill) {
    char symbol;
    int rand_i;

    // Pick a terminal if the max depth has been reached.
    // If `must_fill` is false, the probability
    // that the symbol will be a terminal is 50%.
    if (curr_depth >= (max_depth - 1) || (!must_fill && get_rand_probability() > 0.5)) {
        rand_i = get_randint(0, symbols->term_size - 1);

        symbol = symbols->terminals[rand_i];
    } else {
        // Pick a random function
        rand_i = get_randint(0, symbols->func_size - 1);
        symbol = symbols->functions[rand_i];
    }

    assert(symbol_is_valid(symbol, symbols->arities));

    return symbol;
}

/**
 * Recursively grow a node to max depth in a pre-order, ie. depth-first
 * left-to-right traversal.
 * @param node The node to grow.
 * @param curr_depth The current depth in the tree.
 * @param max_depth The max depth that the tree can grow to.
 * @param must_fill Grows all branches of a tree to max depth when true.
 */
void grow(struct node *node, int curr_depth, int max_depth, bool must_fill) {
    if (curr_depth >= max_depth) return;

    char curr_sym = node->value;
    char curr_sym_str[2] = {curr_sym, '\0'};
    char new_sym;

    // The grow function is called recursively in the loop. The loop iterates
    // <arity> number of times, as defined in the arities hashmap.
    for (int side = 0; side < get_hashmap(symbols->arities, curr_sym_str); side++) {
        new_sym = get_random_symbol(curr_depth, max_depth, must_fill);

        struct node *new_node = append_node(node, new_sym, side);

        // Call grow with the child node as the current node.
        if (curr_depth + 1 < max_depth && strchr(symbols->functions, new_sym)) {
            grow(new_node, curr_depth + 1, max_depth, must_fill);
        }
    }

    assert(curr_depth <= max_depth);
}

/**
 * Randomly select a node and replace it with a new, randomly generated node.
 * @param root The root node of the tree.
 */
void subtree_mutation(struct node *root) {

    if (get_rand_probability() >= MUTATION_PROBABILITY) return;

    // Pick a node
    int end_node_i = get_number_of_nodes(root) - 1;
    int node_i = get_randint(0, end_node_i);

    struct node *old_node = get_node_at_index_wrapper(root, node_i);

    // Get new subtree
    int node_depth = get_depth_at_index_wrapper(root, node_i);

    char new_symbol = get_random_symbol(
            node_depth, MAX_DEPTH - node_depth, false
    );

    struct node *new_subtree = new_node(new_symbol);

    grow(new_subtree, node_depth, MAX_DEPTH, false);

    // Replace the old subtree with the new one
    *old_node = *new_subtree;
}

/**
 * Swap two random nodes from the two given trees.
 * @param p1 A tree to crossover.
 * @param p2 The other tree to crossover.
 * @return The new trees.
 */
struct node **subtree_crossover(struct node *p1, struct node *p2) {

    struct node *parent1 = tree_deep_copy(p1);
    struct node *parent2 = tree_deep_copy(p2);

    if (get_rand_probability() < CROSSOVER_PROBABILITY) {
        struct node *xo_nodes[2];
        int node_depths[2][2];

        for (int i = 0; i < 2; i++) {
            struct node *parent = (i ? parent2 : parent1);

            // Pick a crossover point.
            int node_i = get_randint(0, get_number_of_nodes(parent) - 1);

            // Find the subtree at the crossover point
            xo_nodes[i] = get_node_at_index_wrapper(parent, node_i);

            node_depths[i][0] = get_depth_at_index_wrapper(parent, node_i);
            node_depths[i][1] = get_max_tree_depth(xo_nodes[i]);
        }

        // Ensure the trees will not exceed max depth.
        if ((node_depths[0][1] + node_depths[1][0] > MAX_DEPTH) ||
            (node_depths[1][1] + node_depths[0][0] > MAX_DEPTH)) {
            if (VERBOSE) fprintf(stderr, "Crossover too deep.\n");
        } else {
            // Swap the nodes

            struct node tmp = *xo_nodes[0];

            *xo_nodes[0] = *xo_nodes[1];
            *xo_nodes[1] = tmp;

            assert(
                    get_max_tree_depth(xo_nodes[0]) <= MAX_DEPTH &&
                    get_max_tree_depth(xo_nodes[1]) <= MAX_DEPTH
            );
        }
    }

    struct node **new_parents = allocate_m(sizeof(struct node *) * 2);
    new_parents[0] = parent1;
    new_parents[1] = parent2;

    return new_parents;
}

/**
 * Allocate space for a new individual.
 * @param genome The individual's tree.
 * @param fitness The fitness value of the genome.
 * @return The new individual.
 */
struct individual *new_individual(struct node *genome, double fitness) {
    struct individual *i = allocate_m(sizeof(struct individual));

    i->genome = genome;
    i->fitness = fitness;

    return i;
}

/**
 * Free the memory allocated for an individual and it's genome.
 * @param i The individual to free.
 */
void free_individual(struct individual *i) {
    free_node(i->genome);
    free_pointer(i);
}

/**
 * Print an individuals genome and fitness to the console.
 * @param i The individual.
 */
void print_individual(struct individual *i) {
    printf("Genome: ");
    print_nodes_index_order(i->genome);
    printf(", Fitness: %.4f", i->fitness);
}

/**
 * Evaluate a node recursively. The node's symbol is evaluated.
 * @param node The node to evaluate.
 * @param fitness_case Data to input into variables (defined in csv file).
 * @return The value of the node on the given data.
 */
double evaluate(struct node *node, double *fitness_case) {

    if (!node) return DEFAULT_FITNESS;

    char symbol = node->value;

    if (symbol == '+') {
        return evaluate(node->left, fitness_case) + evaluate(node->right, fitness_case);
    } else if (symbol == '-') {
        return evaluate(node->left, fitness_case) - evaluate(node->right, fitness_case);
    } else if (symbol == '*') {
        return evaluate(node->left, fitness_case) * evaluate(node->right, fitness_case);
    } else if (symbol == '/') {
        double numerator = evaluate(node->left, fitness_case);
        double denominator = evaluate(node->right, fitness_case);

        if (fabs(denominator) < 0.00001) {
            denominator = 1.0;
        }

        return numerator / denominator;
    } else if (isalpha(symbol)) {
        // Fitness case variables must be in alphabetical order
        // for this to work correctly.
        if (symbol >= 'A') {
            return fitness_case[symbol - 'a'];
        } else {
            return fitness_case[symbol - 'A'];
        }
    } else {
        return (double) (symbol - '0');
    }
}

/**
 * Evaluate fitness by comparing the error between the output
 * of an individual (symbolic expression) and the target values.
 * Evaluates and sets the fitness in an individual.
 * Fitness is the negative mean square error (MSE).
 * @param ind The individual to evaluate.
 */
void evaluate_individual(struct individual *ind) {
    double fitness = 0.0; // Initial fitness value

    // Calculate the error between the expected value (training_targets[i])
    // and the actual value (output).
    for (int i = 0; i < training_len; i++) {
        double output = evaluate(ind->genome, training_cases[i]);

        // Get the squared error
        double error = output - training_targets[i];

        fitness += error * error;
    }

    // Get the mean fitness and assign it to the individual.
    ind->fitness = (fitness * -1) / (double) targets_len;

    assert(ind->fitness <= 0);
}

/**
 * Ramped half-half initialization. The individuals in the population
 * are initialized using the grow or the full method for each depth
 * value (ramped) up to max_depth.
 * @param pop The population to initialize.
 */
void init_population(struct individual **pop) {
    bool full;
    int max_depth;
    char symbol;

    for (int i = 0; i < POPULATION_SIZE; i++) {

        // Pick full or grow method
        full = get_randint(0, 1);

        // Ramp the depth
        max_depth = (i % MAX_DEPTH) + 1;

        // Create the root node.
        symbol = get_random_symbol(0, max_depth, full);
        struct node *genome = new_node(symbol);

        // Grow the tree if the root is a function symbol
        if (max_depth > 0 && strchr(symbols->functions, symbol)) {
            grow(genome, 0, max_depth, full);

            assert(get_max_tree_depth(genome) < max_depth + 1);
        }

        struct individual *ind = new_individual(genome, DEFAULT_FITNESS);

        pop[i] = ind;
    }
}

/**
 * Evaluate each individual of a population.
 * Uses a simple cache for reducing the number of evaluations of
 * each individual.
 * @param pop The population to evaluate.
 */
void evaluate_population(struct individual **pop) {
    for (int i = 0; i < POPULATION_SIZE; i++) {
        char *key = tree_to_string(pop[i]->genome);
        double fitness = get_hashmap(pop_cache, key);

        if (!isnan(fitness)) {
            pop[i]->fitness = fitness;
        } else {
            evaluate_individual(pop[i]);
            put_hashmap(pop_cache, key, pop[i]->fitness);
        }
    }
}

/**
 * Sort population in reverse order order with regards to fitness.
 * @param pop The population to sort.
 * @param size The size of the population.
 */
void sort_population(struct individual **pop, int size) {
    qsort(pop, size, sizeof(*pop), fitness_comp);
}

/**
 * Helper function to compare individuals in term of their fitness.
 * Use with `qsort`.
 * @param elem1, elem2 The elements to compare.
 * @return Return 1 if `elem2` has a greater fitness,
 * return -1 if `elem1` has a greater fitness and return 0 if they
 * are equal.
 */
int fitness_comp(const void *elem1, const void *elem2) {
    struct individual *i1 = *(struct individual **) elem1;
    struct individual *i2 = *(struct individual **) elem2;

    if (i1->fitness < i2->fitness) return 1;
    if (i1->fitness > i2->fitness) return -1;

    return 0;
}

/**
 * Print the genome and fitness of each individual in a population.
 * @param pop The population to print.
 * @param size The size of the population.
 */
void print_population(struct individual **pop, int size) {
    for (int i = 0; i < size; i++) {
        print_individual(pop[i]);
        printf("\n");
    }
}

/**
 * Print the statistics of a population.
 * @param generation The current generation.
 * @param pop The population.
 * @param duration Duration of computation.
 */
void print_stats(int generation, struct individual **pop, double duration) {
    sort_population(pop, POPULATION_SIZE);

    if (VERBOSE) {
        printf("-------------POPULATION:-------------\n");
        print_population(pop, POPULATION_SIZE);
    }

    double *fitness_values = allocate_m(sizeof(double) * POPULATION_SIZE);
    double *size_values = allocate_m(sizeof(double) * POPULATION_SIZE);
    double *depth_values = allocate_m(sizeof(double) * POPULATION_SIZE);

    for (int i = 0; i < POPULATION_SIZE; i++) {
        fitness_values[i] = pop[i]->fitness;
        size_values[i] = (double) get_number_of_nodes(pop[i]->genome);
        depth_values[i] = (double) get_max_tree_depth(pop[i]->genome);
    }

    double *fit_stats = get_ave_and_std(fitness_values, POPULATION_SIZE);
    double *size_stats = get_ave_and_std(size_values, POPULATION_SIZE);
    double *depth_stats = get_ave_and_std(depth_values, POPULATION_SIZE);

    int max_size = (int) max_value(size_values, POPULATION_SIZE);
    int max_depth = (int) max_value(depth_values, POPULATION_SIZE);
    double max_fitness = max_value(fitness_values, POPULATION_SIZE);

    printf(
            "Generation: %d, "
                    "Duration: %.4f, "
                    "fit ave: %.2f+/-%.3f, "
                    "size ave: %.2f+/-%.3f "
                    "depth ave: %.2f+/-%.3f, "
                    "max size: %d, "
                    "max depth: %d, "
                    "max fit: %f, "
                    "best solution: ",
            generation,
            duration,
            fit_stats[0], fit_stats[1], size_stats[0], size_stats[1],
            depth_stats[0], depth_stats[1], max_size, max_depth, max_fitness
    );

    print_individual(pop[0]);
    printf("\n");

    free_pointer(fitness_values);
    free_pointer(size_values);
    free_pointer(depth_values);
    free_pointer(fit_stats);
    free_pointer(size_stats);
    free_pointer(depth_stats);
}

/**
 * Return individuals from a population by drawing `TOURNAMENT_SIZE`
 * competitors randomly and selecting the best of the competitors.
 * `POPULATION_SIZE` number of tournaments are held.
 * @param pop The population the select from.
 * @return The winners of the tournaments.
 */
struct individual **tournament_selection(struct individual **pop) {
    struct individual **winners = allocate_m(sizeof(struct individual *) * POPULATION_SIZE);

    int win_i = 0;
    struct individual **competitors = allocate_m(sizeof(struct individual *) * TOURNAMENT_SIZE);

    while (win_i < POPULATION_SIZE) {

        for (int i = 0; i < TOURNAMENT_SIZE; i++) {
            int idx = get_randint(0, POPULATION_SIZE - i - 1);
            competitors[i] = pop[idx];

            // Swap competitor with last element
            // Ensure that they will not be picked again.
            struct individual *tmp = pop[POPULATION_SIZE - i - 1];
            pop[POPULATION_SIZE - i - 1] = pop[idx];
            pop[idx] = tmp;
        }

        sort_population(competitors, TOURNAMENT_SIZE);

        winners[win_i++] = competitors[0];
    }

    free_pointer(competitors);

    return winners;
}

/**
 * Return a new population. The `ELITE_SIZE` best of the old population
 * replace the `ELITE_SIZE` worst of the new population if their fitness
 * is higher.
 * @param new_pop The new population
 * @param old_pop The old population
 */
void generational_replacement(struct individual **new_pop, struct individual **old_pop) {
    sort_population(new_pop, POPULATION_SIZE);
    sort_population(old_pop, POPULATION_SIZE);

    for (int i = 0; i < ELITE_SIZE; i++) {
        if (old_pop[i]->fitness > new_pop[POPULATION_SIZE - i - 1]->fitness) {
            // Free unused individuals.
            free_individual(new_pop[POPULATION_SIZE - i - 1]);
            new_pop[POPULATION_SIZE - i - 1] = old_pop[i];

            old_pop[i] = NULL; // Set to NULL to ensure that it is not double freed.
        }
    }
}

/**
 * Get the best individual from the evolutionary search loop,
 * starting for an initial population.
 * @param pop The initial population.
 * @return The best individual.
 */
struct individual *search_loop(struct individual **pop) {

    /////////////////////
    //Evaluate Fitness //
    /////////////////////
    double time = get_time();

    evaluate_population(pop);

    print_stats(0, pop, get_time() - time);

    // Set best solution
    sort_population(pop, POPULATION_SIZE);
    struct individual *best_ever = pop[0];

    int generation = 1;

    struct individual **new_pop = allocate_m(sizeof(struct individual *) * POPULATION_SIZE);
    struct individual **parents;

    /////////////////////
    // Generation Loop //
    /////////////////////

    while (generation < GENERATIONS) {
        time = get_time();

        int new_pop_i = 0;

        ///////////////
        // Selection //
        ///////////////

        parents = tournament_selection(pop);

        ///////////////////////////////////////////////////
        // Variation -- Generate new individual solutions //
        ///////////////////////////////////////////////////

        // Crossover
        while (new_pop_i < POPULATION_SIZE) {
            int idx = get_randint(0, POPULATION_SIZE - 1);
            struct individual *p1 = parents[idx];
            struct individual *p2;

            // Swap p1 with the last element so that it cannot be picked again
            struct individual *tmp = parents[POPULATION_SIZE - 1];

            parents[POPULATION_SIZE - 1] = p1;
            parents[idx] = tmp;

            p2 = parents[get_randint(0, POPULATION_SIZE - 2)];

            struct node **children = subtree_crossover(p1->genome, p2->genome);

            // Append the first child to the population.
            struct individual *i1 = new_individual(children[0], DEFAULT_FITNESS);
            evaluate_individual(i1);
            new_pop[new_pop_i++] = i1;

            // Ensure that too many elements can't be added.
            // Handles uneven population sizes, since crossover returns 2 offspring.
            if (new_pop_i < POPULATION_SIZE) {
                struct individual *i2 = new_individual(children[1], DEFAULT_FITNESS);
                evaluate_individual(i2);
                new_pop[new_pop_i++] = i2;
            }
        }

        // Vary the population by mutation
        for (int i = 0; i < POPULATION_SIZE; i++) {
            subtree_mutation(new_pop[i]->genome);
        }

        ////////////////////
        //Evaluate fitness//
        ////////////////////
        evaluate_population(new_pop);

        /////////////////////////////////////////////////////////////////
        // Replacement. Replace individual solutions in the population //
        /////////////////////////////////////////////////////////////////
        generational_replacement(new_pop, pop);

        swap_populations(&new_pop, &pop);

        for (int i=0; i < POPULATION_SIZE; i++) {
            if (new_pop[i]) {
                free_individual(new_pop[i]);
            }
        }

        // Set best solution
        sort_population(pop, POPULATION_SIZE);
        best_ever = pop[0];

        // Print the Stats of the population
        print_stats(generation, pop, get_time() - time);

        generation++;
    }

    return best_ever;
}

/**
 * Swap the pointers of two populations.
 * @param pop1, pop2 The populations to swap.
 */
void swap_populations(struct individual ***pop1, struct individual ***pop2) {
    struct individual **tmp = *pop1;
    *pop1 = *pop2;
    *pop2 = tmp;
}

/**
 * Out-of-sample test on an individual solution.
 * @param i The solution to test.
 */
void out_of_sample_test(struct individual *i) {
    evaluate_individual(i);

    printf("Best solution on the test data: ");
    print_individual(i);
    printf("\n");
}
