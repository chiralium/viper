#include "arithmetica.h"

/*
 * The module for working with arrays in viper-language context
 */

#ifndef VIPER_V4_VIARRAY_H
#define VIPER_V4_VIARRAY_H

/* The function will be returned element of array by mode */
Constant * get_by_index(Constant * object, Array ** params);

#endif //VIPER_V4_VIARRAY_H
