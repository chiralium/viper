#include "parser.h"
#include "expression.h"


/*
 * The module that provide the internal function mechanism
 */
#ifndef VIPER_V4_FUNCTIONS_H
#define VIPER_V4_FUNCTIONS_H

/* Exception message */
#define FUNCTIONS_INVALID_ARG_LIST "invalid input arguments list for function <%s>"

/* The function will be performing the local namespace by function object & function call */
Node * performing_local_namespace(Array ** input_arguments, Function * function_object);

/* The function will be validate function call by function object that stored in namespace */
int validate_function_call(FuncCall * function_call, Function * function_object);

/* The function does copy a function code */
Array ** copy_function_code(Array ** function_code);

/* The function for destruction the function code from start position */
void function_code_destructor(Array ** function_code, int start_position);

/* The function does calculating the function signature by input arguments */
int get_function_signature(Array ** arg_list);

#endif //VIPER_V4_FUNCTIONS_H
