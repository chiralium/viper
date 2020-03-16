#include <stdio.h>
#include <string.h>
#include "array.h"

/*
 * This header file contained the simple arithmetical functions which assigned with tokens
 */

#ifndef VIPER_V4_ARITHMETICA_H
#define VIPER_V4_ARITHMETICA_H

#define ARITHMETICA_PLUS "+"
#define ARITHMETICA_SUB  "-"
#define ARITHMETICA_MUL  "*"
#define ARITHMETICA_DIV  "/"
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

/* Operators parameters */
#define ARITMHETICA_MAX_INDEX_PARAM 3

void * _add(void * x, void * y);
void * _sub(void * x, void * y);
void * _mul(void * x, void * y);
void * _div(void * x, void * y);
void * _more(void * x, void * y);
void * _less(void * x, void * y);
void * _moreeq(void * x, void * y);
void * _lesseq(void * x, void * y);
void * _equal(void * x, void * y);
void * _tmp(void * x, void * y);

/* The function will compose the Index-structure by input params */
Index * new_index(void * object, Array ** params, int params_count);

/* The function will compose the FuncCall-structure by input params */
FuncCall * new_func_call(char * name, Array ** arg_list);

/* The function will return a pointer to a function which associated with a token literal */
void * assign_function(char * literal);

/* Destructors */
void index_destructor(Index * index);
void func_call_destructor(FuncCall * funccall);

#endif //VIPER_V4_ARITHMETICA_H
