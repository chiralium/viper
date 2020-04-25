#include "arithmetica.h"
/*
 * The module for support the ViStrings
 */

#ifndef VIPER_V4_VISTRING_H
#define VIPER_V4_VISTRING_H

/* Exceptions messages */
#define VISTRING_RANGE_EXCEPTION "index out of range (string)"

/* Define the function for return */
Constant * get_char_by_index(Constant * object, Array ** params);

/* Provides the <string>[index] */
Constant * get_single_char(Constant * object, Constant * index);

/* Provides the <string>[start, end] */
Constant * get_substr(Constant * object, Constant * start, Constant * end);

/* Provides the string[start, end, step] */
Constant * get_substr_step(Constant * object, Constant * start, Constant * end, Constant * step);

#endif //VIPER_V4_VISTRING_H
