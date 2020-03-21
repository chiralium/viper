#include <stdio.h>
#include <string.h>
#include "array.h"
#include "namespace.h"

/*
 * This header file contained the simple arithmetical functions which assigned with tokens.
 * Tools for parsing the expression: convert to postfix notation and calculate it
 */

#ifndef VIPER_V4_ARITHMETICA_H
#define VIPER_V4_ARITHMETICA_H

#define ARITHMETICA_PLUS "+"
#define ARITHMETICA_SUB  "-"
#define ARITHMETICA_MUL  "*"
#define ARITHMETICA_DIV  "/"
#define ARITHMETICA_POW  "^"
#define ARITHMETICA_NOT  "!"
#define ARITHMETICA_MORE ">"
#define ARITHMETICA_LESS "<"
#define ARITHMETICA_MEQ  ">="
#define ARITHMETICA_LEQ  "<="
#define ARITHMETICA_EQ   "=="
#define ARITHMETICA_ASG  "="
#define ARITHMETICA_OCB  "("
#define ARITHMETICA_CCB  ")"
#define ARITHMETICA_OBB  "{"
#define ARITHMETICA_CBB  "}"
#define ARITHMETICA_OSB  "["
#define ARITHMETICA_CSB  "]"
#define ARITHMETICA_COMA ","

/* The struct of index */
typedef struct Index {
    void * object;;
    Array ** params; // the first element - is a just index of array, if second element is exist - is a stop index, if third element is exist is a step value
    int params_count;
} Index;

/* The struct of call of function */
/* The tokens sequence like CONSTANT(...) -- is a function call */
typedef struct FuncCall {
    char * name;
    Array ** arg_list;
} FuncCall;

/* The data-structure that used for store result of calculate expression */
typedef struct Constant {
    char type_id;
    void * value;
} Constant;

/* Types parameters */
#define ARITHMETICA_MAX_STRING_LEN 255

/* Operators parameters */
#define ARITMHETICA_MAX_INDEX_PARAM 3

/* Exceptions */
#define ARITHMETICA_BRACES_NOT_BALANCED       "invalid syntax of expression, braces is not balanced"
#define ARITHMETICA_INVALID_EXPRESSION_SYNTAX "invalid syntax of expression"
#define ARITHMETICA_INVALID_OPERAND           "invalid type of operand"
#define ARITHMETICA_TYPECASTING_ERROR         "converting from <%c>-type to <%c>-type"
#define ARITHMETICA_UNDEFINED_NAME            "undefined name of variable"
#define ARITHMETICA_STRING_CONCATE_EXCEPTION  "invalid concating operand"

void * _add(void * x, void * y);
void * _sub(void * x, void * y);
void * _mul(void * x, void * y);
void * _div(void * x, void * y);
void * _pow(void * x, void * y);
void * _not(void * x, void * y);
void * _more(void * x, void * y);
void * _less(void * x, void * y);
void * _moreeq(void * x, void * y);
void * _lesseq(void * x, void * y);
void * _equal(void * x, void * y);
void * _asg(void * x, void * y);
void * _tmp(void * x, void * y);

/* The main entry point of module
 * This function will calculate expression and return the value as data-structure
 */
Constant * arithmetica(Array ** expression_tokens, Node * current_namespace);

/* The function will convert the infix notation of expression to postfix notation */
Array ** postfix(Array ** expression_tokens);

/* The function will insert zero-token-value before unary plus or minus */
Array ** fixing_unary_operators(Array ** expression_tokens);

/* The function will crate the zero-token */
void * make_zero_tk();

/* The function will return the priority of operator */
int _get_priority(char * operator);

/* The function will return a pointer to a function which associated with a token literal. Jan Łukasiewicz(c) */
void * assign_function(char * literal);

/* The function will compose the Index-structure by input params */
Index * new_index(void * object, Array ** params, int params_count);

/* The function will compose the FuncCall-structure by input params */
FuncCall * new_func_call(char * name, Array ** arg_list);

/* The function will copy data into heap */
void * copy_data(void * src, char type_id);

/* The function will make constant by input params */
Constant * new_constant(char type_id, void * value);

/* The function will return typecasted value */
int get_int_value(void * token);
float get_float_value(void * toke);

/* The function will getting value from namespace */
int get_from_namespace(void * token);

/* Destructors */
void index_destructor(Index * index);
void func_call_destructor(FuncCall * funccall);
void constant_destructor(Constant * constant);

#endif //VIPER_V4_ARITHMETICA_H
