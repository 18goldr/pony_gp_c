#include "include/main.h"

struct symbols *symbols;

int main() {
    setup();

    run_tests();

    destroy_memory();

    exit(EXIT_SUCCESS);
}

/**
 * Set up memory pool, set seed, define symbols.
 */
void setup() {
    // Initialize memory pool.
    init_memory(DEFAULT_MEMORY_POOL_SIZE);

    // Define symbols
    symbols = allocate_m(sizeof(struct symbols));

    FILE *config = fopen(CONFIG_DIR, "r");
    set_params(config, symbols);
    fclose(config);

    // Set the seed
    set_seed(SEED);
    start_srand();

    FILE *csv = fopen(CSV_DIR, "r");
    csv_add_constants(csv, symbols);
    set_test_and_train_data(csv);
    fclose(csv);

    // TODO implement the rest of this function.
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
 * Unit tests to ensure the program is running.
 * In order for the program to run correctly,
 * the following values must be set:
 *      SEED                  = 2.0
 *      MUTATION_PROBABILITY  = 1.0
 *      CROSSOVER_PROBABILITY = 1.0
 *      MAX_DEPTH             = 5
 */
void run_tests() {
    get_node_at_index_test();
    get_max_tree_depth_test();
    subtree_mutation_test();
    subtree_crossover_test();
}

void get_node_at_index_test() {
    char values[] = {'*', '+', '5', '4', '3'};

    struct node *node = new_node('*');
    node->left = new_node('+');
    node->right = new_node('3');
    node->left->right = new_node('4');
    node->left->left = new_node('5');

    for (int i=0; i < 5; i++) {
        struct node *n = get_node_at_index_wrapper(node, i);

        if (n->value != values[i]) {
            fprintf(stderr, "get_node_at_index has been modified and is broken.\n");
        }
    }

    free_node(node);

}

void get_max_tree_depth_test() {
    struct node *node = new_node('*');
    node->left = new_node('+');
    node->right = new_node('3');
    node->left->right = new_node('4');
    node->left->left = new_node('5');

    if (get_max_tree_depth(node) != 2 || get_max_tree_depth(NULL) != 0) {
        fprintf(stderr, "get_max_tree_depth has been modified and is broken.\n");
    }

    free_node(node);

}

void subtree_mutation_test() {
    char values[] = {'*', '+', '5', '4', '-', '/', '1', 'b', 'b'};

    struct node *node = new_node('*');
    node->left = new_node('+');
    node->right = new_node('3');
    node->left->right = new_node('4');
    node->left->left = new_node('5');

    subtree_mutation(node);

    for (int i=0; i < 9; i++) {
        struct node *n = get_node_at_index_wrapper(node, i);

        if (n->value != values[i]) {
            fprintf(stderr, "subtree_mutation has been modified and is broken.\n");
        }
    }

    free_node(node);
}

void subtree_crossover_test() {
    char node_values[] = {'/', '+', '4', '2', '9'};
    char node1_values[] = {'*', '+', '5', '1', '3'};

    struct node *node = new_node('*');
    node->left = new_node('+');
    node->right = new_node('3');
    node->left->right = new_node('4');
    node->left->left = new_node('5');

    struct node *node1 = new_node('/');
    node1->left = new_node('+');
    node1->right = new_node('9');
    node1->left->right = new_node('2');
    node1->left->left = new_node('1');

    struct node **nodes = subtree_crossover(node, node1);


    for (int i=0; i < 5; i++) {
        struct node *n = get_node_at_index_wrapper(nodes[1], i);

        if (n->value != node_values[i]) {
            fprintf(stderr, "subtree_crossover has been modified and is broken.\n");
        }
    }

    for (int i=0; i < 5; i++) {
        struct node *n = get_node_at_index_wrapper(nodes[0], i);

        if (n->value != node1_values[i]) {
            fprintf(stderr, "subtree_crossover has been modified and is broken.\n");
        }
    }

    free_node(node);
    free_node(node1);
}