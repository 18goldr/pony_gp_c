#include "../include/memmngr.h"

static void resize_memory(size_t size, size_t new_max_size);
static void append_memory(void *p);
static void print_malloc_error(void);

static void **memory;
static int num_elements = 0;
static size_t max_elements;
static bool initialized = false;

/**
 * Get whether memory has been initialized.
 */
bool is_initialized() {
    return initialized;
}

/**
 * Get the current max number of elements.
 */
size_t get_current_max_size() {
    return max_elements;
}

/**
 * Get the current number of elements allocated.
 */
int get_num_elements() {
    return num_elements;
}

/**
 * Initialize the memory pool.
 * @param size The size (bytes) to allocate to the memory pool.
 */
void init_memory(size_t size) {
    memory = malloc(size * sizeof(void *));

    if (!memory) {
        print_malloc_error();
    } else {
        max_elements = size;
        initialized = true;
    }
}

/**
 * Resize the memory pool size.
 * @param size The size (bytes) to reallocate to the memory pool.
 */
static void resize_memory(size_t size, size_t new_max_size) {
    memory = realloc(memory, size);

    if (!memory) {
        fprintf(stderr, "Reallocation failed.");
    } else {
        max_elements = new_max_size;
        if (VERBOSE) {
            printf("New size: %d\n", (int)max_elements);
        }
    }
}

/**
 * Allocate memory. Memory is defined with malloc then added to an array.
 * @param size The size in bytes of how much memory to allocate.
 * @return If successful, return the pointer to the lowest byte in the allocated memory block.
 */
void *allocate_m(size_t size) {
    if (initialized) {
        void *p = malloc(size);
        if (!p) print_malloc_error();
        else append_memory(p);

        return p;

    } else {
        printf("Memory not initialized.");
    }
}

/**
 * Append a pointer at <num_elements> to the memory.
 * @param p The pointer to append.
 */
static void append_memory(void *p) {
    if (max_elements < num_elements) {
        size_t max = max_elements * 2;
        resize_memory(max * sizeof(void *), max);
    }

    memory[num_elements++] = p;
}

/**
 * Free all the memory in the memory pool.
 */
void free_all() {
    if (initialized) {
        if (num_elements) {
            for (int i = 0; i < num_elements; i++) {
                if (VERBOSE) printf("Delete: %p\n", memory[i]);

                free(memory[i]);
            }
            // Reset number of elements.
            num_elements = 0;

        } else if (VERBOSE){
            printf("Memory pool is already empty.");
        }
    } else {
        fprintf(stderr, "Memory not initialized.");
    }
}

/**
 * Free the memory pool.
 */
void deinit_memory() {
    if (initialized) {
        free(memory);
        initialized = false;
    }
}

/**
 * Deallocate all memory in the memory pool and the memory pool itself.
 */
void destroy_memory() {
    free_all();
    deinit_memory();
}

/**
 * Print an error for when malloc returns Null.
 */
void print_malloc_error() {
    fprintf(stderr, "Memory allocation failed.");
}

/**
 * Free an allocated memory address.
 * @param p The pointer to the memory address.
 */
void free_pointer(void *p) {
    bool found = false;
    for (int i=0; i < num_elements; i++) {
        if (p == memory[i]) {
            found = true;
            free(memory[i]);

            // Fill the space of the freed pointer with the last memory address in the memory pool.
            memory[i] = memory[num_elements-1];
            memory[num_elements-1] = NULL;

            num_elements--;
        }
    }

    if (!found) printf("Memory address not found.\n");
}