#ifndef PONY_GP_MEMMNGR_H
#define PONY_GP_MEMMNGR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/params.h"

#define DEFAULT_MEMORY_POOL_SIZE 1000000 // 1 megabyte

bool is_initialized(void);
size_t get_current_max_size(void);
int get_num_elements(void);
void init_memory(size_t size);
void *allocate_m(size_t size);
void destroy_memory(void);
void free_all(void);
void deinit_memory(void);
void free_pointer(void *p);

#endif //PONY_GP_MEMMNGR_H
