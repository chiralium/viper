#include "arithmetica.h"

/*
 * The module for working with arrays in viper-language context & string
 */

#ifndef VIPER_V4_VIARRAY_H
#define VIPER_V4_VIARRAY_H

/* Exception messages */
#define VIARRAY_INDEX_EXCEPTION  "unacceptable index value"
#define VIARRAY_RANGE_EXCEPTION  "index out of range"
#define VIARRAY_NEW_ELEMENT_ERR  "create new element available only for arrays"
#define VIARRAY_UNDEFINED_KEY    "undefined key-value"
#define VIARRAY_INVALID_KEY_TYPE "invalid key type"

/* The function will be allocate the system array into heap */
Node * new_viarray(Array ** array);

/* Display viarray */
void display_viarray(Node * root);

/* The function will be return element or sub-array by index */
Constant * get_by_index(Constant * object, Array ** params);

/* The function will be return single element by index */
Constant * get_single(Constant * object, Constant * index);

/* The function will be return the sub-viarray by parameters */
Constant * get_subviarray(Constant * object, Constant * start, Constant * end);

/* The function will be return the sub-viarray by parameters and step */
Constant * get_subviarray_step(Constant * object, Constant * start, Constant * end, Constant * step);

/* The function will be create new node of viarray and return the origin */
Constant * get_new(Constant * object);

/* The function will be return the length of viarray */
int get_length(Node * viarray);

/* The function will copy ViArray */
Node * copy_viarray(Node * viarray);

/* The function will be validate the type of index parameter */
void validate_index_parameter(Array ** params);

#endif //VIPER_V4_VIARRAY_H
