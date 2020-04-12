#include "arithmetica.h"
#include "expression.h"
#include "namespace.h"

/*
 * This module will interpretate the language constructions.
 * All simple instructions (expression) will be received to arithmetica module.
 * All complex instruction (like IF, WHILE, etc.) will be performed in the module.
 */
#ifndef VIPER_V4_INTERPRETER_H
#define VIPER_V4_INTERPRETER_H

/* Main entry point to program executing */
Constant * main_entry(char * input_stream);

/* Main entry point to program parsing */
Array ** main_parsing(char * input_stream);

/* Interprete the global code structures */
Constant * interpreter(Array ** code, Node * current_namespace);

/* The function will be send expression to arithmetica-module */
Constant * calculate_expression(Array ** expression, Node * current_namespace);

/* The function will be store declared function into namespace */
void function_declaration(Function * function_object, Node * current_namespace);

/* The function will be execute the vipers-functions locally */
Constant * function_exec(Array ** function_code, Node * local_namespace);

/* The function will be execute the return-statement locally */
Constant * return_exec(char * return_expression, Node * local_namespace);

/* Display the callstack */
void display_callstack(Array ** points);

/* The function will be create the meta-data in namespace */
Node * meta_data();

#endif //VIPER_V4_INTERPRETER_H
