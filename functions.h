#include "parser.h"
#include "expression.h"


/*
 * The module that provide the internal function mechanism
 */
#ifndef VIPER_V4_FUNCTIONS_H
#define VIPER_V4_FUNCTIONS_H

/* The structure for storing the functions */
typedef struct FunctionContainer {
    char * name;
    Node * overloaded_functions;
} FunctionContainer;

/* Exception message */
#define FUNCTIONS_INVALID_ARG_LIST     "invalid input arguments list for function <%s>"
#define FUNCTIONS_REDEFINITION_WARNING "the function was redefine"
#define FUNCTIONS_OVERLOADING_WARNING  "the function was overloaded"

/* The function will be create new function container by name */
FunctionContainer * new_function_container(char * name, Node * functions);

/* The function will be performing the local namespace by function object & function call */
Node * performing_local_namespace(Array ** input_arguments, Function * function_object);

/* The function will be validate function call by function object that stored in namespace */
int validate_function_call(FuncCall * function_call, Function * function_object);

/* The function does copy a function code */
Array ** copy_function_code(Array ** function_code);

/* The function for destruction the function code from start position */
void function_code_destructor(Array ** function_code, int start_position);

/* The function for destruction the function container */
void function_container_destructor(FunctionContainer * function_container);

/* The function for destruction overloaded function tree in container */
void overloaded_function_tree_destructor(Node * overloaded_function);

/* The function does calculating the function signature by input arguments */
int get_function_signature(Array ** arg_list);

#endif //VIPER_V4_FUNCTIONS_H
