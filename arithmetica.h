#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "array.h"
#include "namespace.h"
#include "parser.h"

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
#define ARITHMETICA_ASC  "=>"
#define ARITHMETICA_EXT  "->"
#define ARITHMETICA_IS   "<=>"

/* The struct of index */
typedef struct Index {
    void * object;
    Array ** params; // the first element - is a just index of array, if second element is exist - is a stop index, if third element is exist is a step value
    int params_count;
} Index;

/* The struct of call of function */
/* The tokens sequence like CONSTANT(...) -- is a function call */
typedef struct FuncCall {
    Array ** function_pointer;
    Array ** arg_list;
} FuncCall;

/* The data-structure that used for store result of calculate expression */
typedef struct Constant {
    char type_id;
    void * value;
    void * origin;
} Constant;

/* The data-structure like ExpressionToken but with extra fields */
/* New data structure necessary for few reason:
 * 1. Mixing absolutely differ structure by value is a not good way (ExpressionToken is a token -- result of parsing)
 * 2. Extra fields necessary to indicating the origin of element (for example, some element of expression is a element of array and etc.)
 * 3. Constant structure is a structure of result of work this module
 * 4. Element structure is a intermediate structure that used in calculations
 */
typedef struct Element {
    char * literal;
    char type_id;
    void * value;
    char vtype_id;
    void * origin;   // the pointer to the same element in parent structure
} Element;

/* Types parameters */
#define ARITHMETICA_MAX_STRING_LEN 255

/* Operators parameters */
#define ARITMHETICA_MAX_INDEX_PARAM 3

/* Exceptions */
#define ARITHMETICA_BRACES_NOT_BALANCED       "invalid syntax of expression, braces is not balanced"
#define ARITHMETICA_INVALID_EXPRESSION_SYNTAX "invalid syntax of expression"
#define ARITHMETICA_INVALID_OPERAND           "invalid type of operand <%s>"
#define ARITHMETICA_TYPECASTING_ERROR         "converting from <%s>-type to <%s>-type"
#define ARITHMETICA_UNDEFINED_NAME            "undefined name of variable"
#define ARITHMETICA_STRING_CONCATE_EXCEPTION  "invalid concating operand"
#define ARITHMETICA_UNDEFINED_OPERATOR        "undefined operator (something wrong with expression parser)"
#define ARITHMETICA_SYNTAX_EXCEPTION          "syntax error"
#define ARITHMETICA_NOT_ITERABLE_EXCEPTION    "the object is not iterable"
#define ARITHMETICA_OBJECT_NOT_ASSIGNABLE     "the object is not assignable"
#define ARITHMETICA_OBJECT_NOT_HASHABLE       "the object is not hashable"
#define ARITHMETICA_OBJECT_NOT_CALLABLE       "the object is not callable"
#define ARITHMETICA_OBJECT_NOT_EXTRACTABLE    "the object is not extractable"
#define ARITHMETICA_INVALID_NAME              "invalid name of attribute"

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
void * _asc(void * x, void * y);
void * _ext(void * x, void * y);
void * _tmp(void * x, void * y);
void * _asg(void * x, void * y);
void * _is(void * x, void * y);

/* Some sort of assignments functions */

    /* assignment new variable to simple data */
void _asg_from_data_to_data(Element * y, Element * x);
    /* assignment new varialbe to pointer */
void _asg_from_data_to_pointer(Element * y, Element * x);
    /* assignment pointer to simple data */
void _asg_from_pointer_to_data(Element * y, Element * x);
    /* assignment pointer to pointer */
void _asg_from_pointer_to_pointer(Element * y, Element *x);



/* Main entry point to the module */
Constant * arithmetica_wrapper(Array ** expression_tokens, Node * current_namespace);

/*
 * This function will calculate expression and return the value as data-structure
 */
Constant * arithmetica(Array ** expression_tokens, Node * current_namespace);

/* The function will convert the infix notation of expression to postfix notation */
Array ** postfix(Array ** expression_tokens);

/* The function will insert zero-token-value before unary plus or minus */
Array ** fixing_unary_operators(Array ** expression_tokens);

/* The function will convert the ExpressionToken to the Element-structure */
Element * convert_to_element(void * token);

/* The function will calculated the arrays element */
Array ** array_precalc(Array ** array);

/* The function will calculated the index's element */
Constant * index_precalc(Index * index);

/* The function will calculated the function */
Constant * function_precalc(FuncCall * function_call);

/* The function will crate the zero-token */
void * make_zero_tk();

/* The function will return the priority of operator */
int _get_priority(char * operator);

/* The function will return a pointer to a function which associated with a token literal. Jan Łukasiewicz(c) */
void * assign_function(char * literal);

/* The function will compose the Index-structure by input params */
Index * new_index(void * object, Array ** params);

/* The function will compose the FuncCall-structure by input params */
FuncCall * new_func_call(Array ** function_pointer, Array ** arg_list);

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
void element_destructor(Element * elexpr);

/* Return a true if the type is an simple data */
int is_simple_data(char type_id);

/* Return a true if the element has type CONSTANT */
int is_constant(char type_id);

void * _asg_old(void * x, void * y);
#endif //VIPER_V4_ARITHMETICA_H
