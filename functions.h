#include "parser.h"
#include "expression.h"

/*
 * The module that provide the internal function mechanism
 */
#ifndef VIPER_V4_FUNCTIONS_H
#define VIPER_V4_FUNCTIONS_H

/* Exception message */
#define FUNCTIONS_INVALID_ARG_LIST "invalid input argument list for function <%s>"

/* The function will be validate function call by function object that stored in namespace */
int validate_function_call(FuncCall * function_call, Function * function_object);

#endif //VIPER_V4_FUNCTIONS_H
