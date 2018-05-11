#include "../include/binary_tree.h"

/**
 * Allocate memory for a binary tree node.
 * Set the value of the node and set children to NULL.
 * @param v The value of the node.
 * @return The newly allocated node.
 */
struct node *new_node(char v) {
    struct node *node = allocate_m(sizeof(struct node));

    node->value = v;
    node->left = NULL;
    node->right = NULL;

    return node;
}

/**
 * Free the memory allocated for a binary tree node.
 * @param node The node to free.
 */
void free_node(struct node *node) {
    if (VERBOSE) {
        printf("Delete: %p\n  Node: ", node);
        print_nodes_index_order(node);
        printf("\n");
    }

    if (node->left) free_node(node->left);
    if (node->right) free_node(node->right);

    free_pointer(node);
}

/**
 * Get the number of nodes in a binary tree through the
 * Morris Tree Traversal algorithm.
 * @param root The root of the tree.
 * @return The number of nodes in the binary tree.
 */
int get_number_of_nodes(struct node *root) {
    if (!root) return 0;

    int count = 0;
    struct node *pre, *current = root;

    while (current) {
        if (!current->left) {
            count++;
            current = current->right;
        }
        else {
            pre = current->left;

            while (pre->right && pre->right != current) {
                pre = pre->right;
            }

            if (!pre->right) {
                pre->right = current;
                current = current->left;

            }
            else {
                pre->right = NULL;
                count++;
                current = current->right;
            }
        }
    }

    return count;
}

/**
 * Return an array of the children of a binary tree.
 * @param root The root of the binary tree.
 * @return An array of children nodes.
 */
struct node **get_children(struct node *root) {
    short l = 0, r = 0;

    // Check that the root has a left and right descendant.
    if (root->left)  l = 1;
    if (root->right) r = 1;

    // If neither, it has no children.
    if (!(r + l)) return NULL;

    // Determine array capacity based on the number of children;
    struct node **children = allocate_m((r + l) * sizeof(struct node *));

    int i = 0;

    if (l) children[i++] = root->left;
    if (r) children[i] = root->right;

    return children;
}

/**
 * Get the number of children of a node. The children
 * are the node's immediate descendants.
 * @param root The node to check.
 * @return The number of children of the given node.
 */
int get_num_children(struct node *root) {
    return (root->left ? 1 : 0) + (root->right ? 1 : 0);
}

/**
 * Get the depth of a node in relation to the root of the tree based on the index.
 * The index is based on depth-first left-to-right traversal.
 * @param root The root tree node.
 * @param goal_i The index to search for.
 * @param curr_i The current index in the tree.
 * @param curr_depth The current depth in the tree.
 * @param i_depth The max depth so far.
 * @return The depth of the node at the given index.
 */
int get_depth_at_index(struct node *root, int goal_i,
                       int *curr_i, int curr_depth, int *i_depth) {
    if (goal_i < 0) return -1;
    if (goal_i == 0) return 0;

    if (goal_i == *curr_i) *i_depth = curr_depth;

    struct node **children = get_children(root);

    for (int i=0; (*i_depth == -1) && i < get_num_children(root); i++) {
        (*curr_i)++;

        *i_depth = get_depth_at_index(children[i], goal_i, curr_i, curr_depth + 1, i_depth);
    }

    if (children) free_pointer(children);

    return *i_depth;
}

/**
 * A wrapper for get_depth_at_index().
 * @param root The root of the tree.
 * @param goal_i The index to search for.
 * @return The depth of the node at the given index.
 */
int get_depth_at_index_wrapper(struct node *root, int goal_i) {
    int index = 0;
    int i_depth = -1;

    return get_depth_at_index(root, goal_i, &index, 0, &i_depth);
}

/**
 * Get the node in a tree at a given index. The index is
 * according to a depth-first left-to-right ordering.
 * @param root The root node.
 * @param goal_i The index to search for.
 * @param curr_i The current index in the tree.
 * @param goal The node to be returned.
 * @return The node in the tree at the given index.
 */
struct node *get_node_at_index(
        struct node *root, int goal_i, int *curr_i, struct node **goal) {

    if (goal_i < 0) return NULL;
    if (goal_i == 0) return root;

    struct node **children = get_children(root);

    // Recursively loop through the tree
    // until the index is reached.
    for (int i = 0; i < get_num_children(root); i++) {

        (*curr_i)++;

        if (*curr_i == goal_i) *goal = children[i];
        else get_node_at_index(children[i], goal_i, curr_i, goal);
    }

    if (children) free_pointer(children);

    return *goal;
}

/**
 * A wrapper for get_node_at_index().
 * @param root The root node.
 * @param goal The node to be returned.
 * @return The node in the tree at the given index.
 */
struct node *get_node_at_index_wrapper(struct node *root, int goal) {
    int i = 0;
    int *ip = &i;
    struct node **tmp = allocate_m(sizeof(struct node **));

    struct node *tmp1 = get_node_at_index(root, goal, ip, tmp);

    free_pointer(tmp);

    return tmp1;
}

/**
 * Print the nodes in a tree in index order (ie.
 * top-bottom right-to-left).
 * @param root The root of the tree to print.
 */
void print_nodes_index_order(struct node *root) {
    int num_nodes = get_number_of_nodes(root);

    for (int i = 0; i < num_nodes; i++) {
        printf("'%c'", get_node_at_index_wrapper(root, i)->value);

        if (i != num_nodes - 1) {
            printf(", ");
        }
    }
}

/**
 * Append a child node to the given node.
 * @param node The node to append to.
 * @param value The value of the child.
 * @param side The side of the parent node to append to.
 *                    0 = left side
 *                    1 = right side
 *             Use the macros LEFT_SIDE and RIGHT_SIDE defined
 *             in binary_tree.h
 * @return The new child node.
 */
struct node *append_node(struct node *node, char value, bool side) {
    struct node *new = new_node(value);

    if (side == LEFT_SIDE) {
        if (node->left) return NULL;
        node->left = new;
    }
    else if (side == RIGHT_SIDE) {
        if (node->right) return NULL;
        node->right = new;
    }

    return new;
}

/**
 * Get the max depth of a tree through iterative traversal using a queue.
 * @param root The root of the tree.
 * @return The max depth of the tree.
 */
int get_max_tree_depth(struct node *root) {
    if (!root) return 0;

    struct queue *q = create_queue();

    enqueue(q, root);

    int height = 0;
    int node_count;

    // Get the height of the tree.
    while ((node_count = q->size)) {
        height++;

        while (node_count > 0) {
            struct node *node = q->front->value;

            free_q_node(dequeue(q));

            if (node->left) {
                enqueue(q, node->left);
            }

            if (node->right) {
                enqueue(q, node->right);
            }

            node_count--;
        }
    }

    free_queue(q);

    // Height is one more than depth.
    return height-1;

}

struct node *tree_deep_copy(struct node *node) {
    struct node *copy = NULL;

    if (node) {
        copy = new_node(node->value);
        copy->left = tree_deep_copy(node->left);
        copy->right = tree_deep_copy(node->right);
    }

    return copy;
}
