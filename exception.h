#include <stdio.h>
/*
 * The functions which terminate the program with specific exceptions
 */

#ifndef VIPER_V4_EXCEPTION_H
#define VIPER_V4_EXCEPTION_H

#define EXCEPTION_CODE_STRUCTURE_ERROR "Code structure error at line %d"

/* Getting the line and terminate a program with CODE_STRUCTURE_ERROR at line */
void throw_code_structure_exception(int line);

#endif //VIPER_V4_EXCEPTION_H
