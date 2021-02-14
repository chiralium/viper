#include "arithmetica.h"
#include "namespace.h"

/**
 * The module for the built-ins functions of Viper language.
 * Each function represents the structure with name, pointer & count of input arguments.
 * The type of input arguments is always the container Constant
 */

#ifndef VIPER_V4_BUILTINS_H
#define VIPER_V4_BUILTINS_H

#define BUILTIN_FUNCTION_SETTIMEOUT_INVALID_TYPE "the built-in function `setTimeout` having invalid arg. type"
#define BUILTIN_FUNCTION_LEN_INVALID_TYPE        "the built-in function `len` having invalid arg. type"
#define BUILTIN_FUNCTION_INPUT_INVALID_TYPE      "the built-in function `input` having invalid arg. type"
#define BUILTIN_FUNCTION_TO_STRING_INVALID_TYPE  "the built-in function 'string' having invalid arg. type"
#define BUILTIN_FUNCTION_TO_INTEGER_SYNTAX_ERROR "the string representing of integer value have syntax error"
#define BUILTIN_FUNCTION_TO_INTEGER_INVALID_TYPE "the built-in function 'integer' having invalid arg. type"

#define BT_OUTPUT   "output"
#define BT_STRING   "string"
#define BT_FLOAT    "float"
#define BT_INTEGER  "integer"
#define BT_TIMEOUT  "setTimeout"
#define BT_INPUT    "input"
#define BT_LEN      "len"

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

/* Get length of array */
Constant * len(Constant * value);

/* Typecasting */
Constant * to_string(Constant * value);
Constant * to_int(Constant * value);
Constant * to_float(Constant * value);

/* System */
Constant * timeout(Constant * callback, Constant * delay);

#endif //VIPER_V4_BUILTINS_H
