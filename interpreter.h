#include "arithmetica.h"
#include "namespace.h"

/*
 * This module will interpretate the language constructions.
 * All simple instructions (expression) will be received to arithmetica module.
 * All complex instruction (like IF, WHILE, etc.) will be performed in the module.
 */
#ifndef VIPER_V4_INTERPRETER_H
#define VIPER_V4_INTERPRETER_H

/* Exceptions */
#define INTERPRETER_INVALID_IF_STATEMENT_VALUE "invalid boolean expression"

/* Main entry point to program executing */
Constant * main_entry(char * input_stream);
/* Main entry point to program parsing */
Array ** main_parsing(char * input_stream);


/* Interprete the global code structures */
Constant * interpreter(Array ** code, Node * current_namespace);
/* The function will be send expression to arithmetica-module */
Constant * calculate_expression(Array ** expression, Node * current_namespace);

/* The declaration of calculated namespace */
void namespace_declaration(NameSpaceObject * namespace, Node * current_namespace);
/* The namespace will be execute after declared, but namespace is not destroying */
Constant * namespace_exec(NameSpace * namespace_stmt);

/* The function will be store declared function into namespace */
void function_declaration(Function * function_object, Node * current_namespace);
/* The function will be execute the vipers-functions locally */
Constant * function_exec(Array ** function_code, Node * local_namespace);
/* The function will be execute the return-statement locally */
Constant * return_exec(char * return_expression, Node * local_namespace);

/* The function will be execute the if-statement condition */
Constant * if_condition_exec(char * condition, Node * current_namespace);
/* The function will be execute the if-statement body */
void if_body_exec(Array ** code, Node * current_namespace);
/* The function will be execute all the if-statement from if to else */
void if_statement_exec(If * statement, Node * current_namespace);


/* Display the callstack */
void display_callstack(Array ** points);
/* The function will be display the heap table */
void display_memory_table(Array ** memory_table);

/* The function will be create the meta-data in namespace */
Node * meta_data();
/* The function will return 1 if the current call is return-statement */
int is_return_call(Array ** call_stack);

#endif //VIPER_V4_INTERPRETER_H
