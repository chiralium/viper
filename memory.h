#include "array.h"
#include "interpreter.h"

/*
 * The module for management the memory allocations for Viper's data
 */

#ifndef VIPER_V4_MEMORY_H
#define VIPER_V4_MEMORY_H

/* Memory table element */
typedef struct MemoryElement {
    void * address;
    char type_id;
    char * type;
    char * owner;
    char * meta;
    int is_freed;
} MemoryElement;

/* Create new memory element */
MemoryElement * new_memory_element(char type_id, void * address, char * owner);

/* Display the memory table */
void display_memory_table(Array ** memory_table);

/* Destroy the all allocated data */
void garbage_destructor(Array ** memory_table);


/* Memory element destructor */
void memory_elemeny_destructor(MemoryElement * memel);
#endif //VIPER_V4_MEMORY_H
