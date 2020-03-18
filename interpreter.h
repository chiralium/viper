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

/* The main entry point of module */
void interpreter(Array ** code);

/* The function will be send expression to arithmetica-module */
Constant * calculate_expression(Array ** expression);

/* The function will destruct the array of code */
void code_destructor(Array ** code);

#endif //VIPER_V4_INTERPRETER_H
