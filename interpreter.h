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

/* Interprete the global code structures */
void interpreter(Array ** code);

/* The function will be send expression to arithmetica-module */
Constant * calculate_expression(Array ** expression, Node * current_namespace);

/* Main entry point */
void main_entry(char * single_line);

/* Display the callstack */
void display_callstack(Array ** points);

/* The function will be create the meta-data in namespace */
Node * meta_data();

#endif //VIPER_V4_INTERPRETER_H
