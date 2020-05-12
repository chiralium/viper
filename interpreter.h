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
#define INTERPRETER_INVALID_IF_STATEMENT_VALUE   "invalid boolean expression"
#define INTERPRETER_INVALID_RETURN_STATEMENT     "the return statement is not in a function"
#define INTERPRETER_INVALID_FUNCTION_DECLARATION "the nested function is not available yet"
#define INTERPRETER_INVALID_GLOBAL_SCOPE         "the global statement is not in local scope"
#define INTERPRETER_INVALID_WHILE_CONDITION      "invalid while expression"

/* Call stack point types */
#define INTERPRETER_CALL_STACK_MAIN      0
#define INTERPRETER_CALL_STACK_RETURN    1
#define INTERPRETER_CALL_STACK_FUNCTION  2
#define INTERPRETER_CALL_STACK_NAMESPACE 3
#define INTERPRETER_CALL_STACK_LOOP      4

typedef struct CallStackPoint {
    char * label;
    char point_type;
} CallStackPoint;

/* The function will create the new call stack point */
CallStackPoint * new_call_stack_point(char * label, char point_type);

/* The function will return true if last call stack point is a function */
int is_function_state(void);
/* The function will retturn true if last call stack point is a return statement */
int is_return_state(void);

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
Constant * if_body_exec(Array ** code, Node * current_namespace);
/* The function will be execute all the if-statement from if to else */
Constant * if_statement_exec(If * statement, Node * current_namespace);

/* The function will exec the while-statement */
Constant * while_statement_exec(While * statement, Node * current_namespace);
/* The function will parse the while body */
Array ** while_body_parser(Array ** while_bodt);

/* The function will be replace the local node by node from global namespace */
void global_exec(Global * statement, Node * local_namespace);

/* The function will be create the meta-data in namespace */
Node * meta_data();

#endif //VIPER_V4_INTERPRETER_H
