#include "arithmetica.h"
#include "expression.h"
#include "array.h"

/*
 * The module that assembling the tokens into internal objects like arrays, indexes and simple data
 */

#ifndef VIPER_V4_COMPOSER_H
#define VIPER_V4_COMPOSER_H

/* Types of composer objects */
    /* simple objects */
#define COMPOSER_OBJECT_INT       1
#define COMPOSER_OBJECT_FLOAT     2
#define COMPOSER_OBJECT_STRING    3

    /* complex objects */
#define COMPOSER_OBJECT_ARRAY         4
#define COMPOSER_OBJECT_ARRAY_ELEMENT 5
#define COMPOSER_OBJECT_INDEX         6
#define COMPOSER_OBJECT_FUNCTION      7

    /* system objects */
#define COMPOSER_OBJECT_OPERATOR   8
#define COMPOSER_OBJECT_VARIABLE   9
#define COMPOSER_OBJECT_MATH      10


/* Operators as string */
#define COMPOSER_OPERATOR_PLUS "+"
#define COMPOSER_OPERATOR_SUB  "-"
#define COMPOSER_OPERATOR_MUL  "*"
#define COMPOSER_OPERATOR_DIV  "/"
#define COMPOSER_OPERATOR_POW  "^"
#define COMPOSER_OPERATOR_NOT  "!"
#define COMPOSER_OPERATOR_MORE ">"
#define COMPOSER_OPERATOR_LESS "<"
#define COMPOSER_OPERATOR_MEQ  ">="
#define COMPOSER_OPERATOR_LEQ  "<="
#define COMPOSER_OPERATOR_EQ   "=="
#define COMPOSER_OPERATOR_ASG  "="
#define COMPOSER_OPERATOR_OCB  "("
#define COMPOSER_OPERATOR_CCB  ")"
#define COMPOSER_OPERATOR_OBB  "{"
#define COMPOSER_OPERATOR_CBB  "}"
#define COMPOSER_OPERATOR_OSB  "["
#define COMPOSER_OPERATOR_CSB  "]"
#define COMPOSER_OPERATOR_COMA ","
#define COMPOSER_OPERATOR_ASC  "=>"

/* Composer exceptions */
#define COMPOSER_UNDEFINED_TOKEN           "undefined token in a token list"
#define COMPOSER_INVALID_ARRAY_DECLARATION "invalid array declaration"

/* Main entry point into composer process */
Array ** composer(Array ** token_list);

/* Function that does assembling the tokens into objects */
Array ** object_assembler(Array ** token_list);

/* Compose the token that has token type like CONSTANT_TK, CONSTANT_STRING_TK into simple data object */
Constant * compose_simple_data(ExpressionToken * token);

/* Compose the token list into array object */
Array ** compose_array_object(Array ** token_list, int * starting_token);

/* Compose the token list into single array element */
Array ** compose_array_element(Array ** token_list, int * starting_token);


/* Some tools */
char * get_operator_as_string(void * operator_pointer);
#endif //VIPER_V4_COMPOSER_H
