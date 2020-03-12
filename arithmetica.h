#include <stdio.h>
#include <string.h>
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

/* The function will return a pointer to a function which associated with token literal */
void * assign_function(char * literal);

#endif //VIPER_V4_ARITHMETICA_H
