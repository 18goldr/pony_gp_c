#include "include/main.h"

struct symbols *symbols;

// Cache for fitness evaluation.
struct hashmap *pop_cache;

int main() {
    init_memory(DEFAULT_MEMORY_POOL_SIZE);

    // Testing
    // symbols = allocate_m(sizeof(struct symbols));
    // run_tests(symbols);

    setup();

    struct individual **pop = allocate_m(sizeof(struct individual *) * POPULATION_SIZE);
    init_population(pop);
    evaluate_population(pop);
    sort_population(pop);

    for (int i=0; i < POPULATION_SIZE; i++) {
        printf("%f\n", pop[i]->fitness);
    }

    destroy_memory();
    exit(EXIT_SUCCESS);
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

        for (int i=0; i < 2; i++) {
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
            if (VERBOSE) fprintf(stderr, "Crossover too deep, subtree_crossover may be broken.\n");
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
    }

    else if (symbol == '-') {
        return evaluate(node->left, fitness_case) - evaluate(node->right, fitness_case);
    }

    else if (symbol == '*') {
        return evaluate(node->left, fitness_case) * evaluate(node->right, fitness_case);
    }

    else if (symbol == '/') {
        double numerator = evaluate(node->left, fitness_case);
        double denominator = evaluate(node->right, fitness_case);

        if (fabs(denominator) < 0.00001) {
            denominator = 1.0;
        }

        return numerator / denominator;
    }

    else if (isalpha(symbol)) {
        // Fitness case variables must be in alphabetical order
        // for this to work correctly.
        if (symbol >= 'A') {
            return fitness_case[symbol - 'a'];
        }
        else {
            return fitness_case[symbol - 'A'];
        }
    }

    else {
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
    ind->fitness = (fitness * -1) / (double)targets_len;

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

    for (int i=0; i < POPULATION_SIZE; i++) {

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
    for (int i=0; i < POPULATION_SIZE; i++) {
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
 */
void sort_population(struct individual **pop) {
    qsort(pop, POPULATION_SIZE, sizeof(*pop), fitness_comp);
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
    struct individual *i1 = *(struct individual **)elem1;
    struct individual *i2 = *(struct individual **)elem2;

    if (i1->fitness < i2->fitness) return 1;
    if (i1->fitness > i2->fitness) return -1;

    return 0;
}