#include "arithmetica.h"
#include "namespace.h"

/**
 * The module for the built-ins functions of Viper language.
 * Each function represents the structure with name, pointer & count of input arguments.
 * The type of input arguments is always the container Constant
 */

#ifndef VIPER_V4_BUILTINS_H
#define VIPER_V4_BUILTINS_H

typedef struct BuiltIn {
    char * name;
    void * function_pointer;
    int args;
} BuiltIn;

/* Create the new BuiltIn structure */
BuiltIn * new_builtin(char * name, void * function_pointer, int args);
/* Execute the builtin function */
Constant * builtin_function_execute(BuiltIn * builtin, Array ** input_args);
/* Download the built-in function into namespace */
void import_builtins(Node * namespace);

/* Input/Output */
Constant * output(Constant * value);
Constant * input(Constant * value);

#endif //VIPER_V4_BUILTINS_H
