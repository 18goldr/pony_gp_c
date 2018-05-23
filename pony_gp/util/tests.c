#include "../include/main.h"

void test_setup(struct symbols *s);

void test_setup(struct symbols *s) {

    MUTATION_PROBABILITY = 1.0;
    CROSSOVER_PROBABILITY = 1.0;
    MAX_DEPTH = 5;
    SEED = 2.0;
    POPULATION_SIZE = 100;
    ELITE_SIZE = 2;
    TOURNAMENT_SIZE = 10;
    TEST_TRAIN_SPLIT = 0.7;
    GENERATIONS = 100;

    s->functions = allocate_m(MAX_HASHMAP_SIZE);
    s->terminals = allocate_m(MAX_HASHMAP_SIZE);

    s->functions = "+-/*";
    s->terminals = "01ab";
    s->func_size = 4;
    s->term_size = 4;

    struct hashmap *h = init_hashmap();
    h->keys[0] = "+";
    h->keys[1] = "*";
    h->keys[2] = "/";
    h->keys[3] = "-";
    h->keys[4] = "0";
    h->keys[5] = "1";
    h->keys[6] = "a";
    h->keys[7] = "b";

    h->values[0] = 2;
    h->values[1] = 2;
    h->values[2] = 2;
    h->values[3] = 2;
    h->values[4] = 0;
    h->values[5] = 0;
    h->values[6] = 0;
    h->values[7] = 0;

    h->num_pairs = 8;
    s->arities = h;

    start_srand();
}

void run_tests(struct symbols *s) {
    test_setup(s);

    subtree_mutation_test();
    subtree_crossover_test();
    get_node_at_index_test();
    get_max_tree_depth_test();

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
    char values[] = {'*', '+', '5', '4', '*', '/', '1', 'b', 'b'};

    struct node *node = new_node('*');
    node->left = new_node('+');
    node->right = new_node('3');
    node->left->right = new_node('4');
    node->left->left = new_node('5');

    subtree_mutation(node);

    for (int i=0; i < 9; i++) {
        struct node *n = get_node_at_index_wrapper(node, i);

        if (!n || n->value != values[i]) {
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

        if (!n || n->value != node_values[i]) {
            fprintf(stderr, "subtree_crossover has been modified and is broken.\n");
        }
    }

    for (int i=0; i < 5; i++) {
        struct node *n = get_node_at_index_wrapper(nodes[0], i);

        if (!n || n->value != node1_values[i]) {
            fprintf(stderr, "subtree_crossover has been modified and is broken.\n");
        }
    }

    free_node(node);
    free_node(node1);
}