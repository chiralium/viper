#include "arithmetica.h"

/*
 * The module for working with arrays in viper-language context
 */

#ifndef VIPER_V4_VIARRAY_H
#define VIPER_V4_VIARRAY_H

/* Exception messages */
#define VIARRAY_INDEX_EXCEPTION "unacceptable index value"
#define VIARRAY_RANGE_EXCEPTION "index out of range"

/* The function will be returned element of array by mode */
Constant * _get_by_index(Constant * object, Array ** params);

/* The function will be validate the type of index parameter */
void _validate_index_parameter(Array ** params);

#endif //VIPER_V4_VIARRAY_H