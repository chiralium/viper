#include <stdlib.h>
#include <string.h>
#include "types.h"


/*
 * The header which describing the array structure
 */
#ifndef VIPER_V4_ARRAY_H
#define VIPER_V4_ARRAY_H

typedef struct Array {
    char type_id;
    void *element;
} Array;

/* Create array of empty pointers to Array-structure */
Array ** new_array(void);

/* Reallocated array to new size */
Array ** realloc_array(Array **_array, unsigned long length);

/* Insert into the end new element into array */
Array ** append(Array **_array, char type_id, void *value);

/* Get length of array */
unsigned long _get_len(Array **_array);

/* Copying array X to array Y */
Array ** copy_array(Array **Y, Array **X);

/* Freed memory from heap */
void array_destructor(Array **_array);

#endif //VIPER_V4_ARRAY_H
