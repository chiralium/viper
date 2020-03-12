#include <ctype.h>
#include "fread.h"
#include "parser.h"
#include "operators.h"
#include "array.h"
#include "exception.h"
#include "arithmetica.h"

/*
 * Tools for parsing the type 2 tokens (parser-module).
 */
#ifndef VIPER_V4_EXPRESSION_H
#define VIPER_V4_EXPRESSION_H

/* Tokens type */
#define EXPRESSION_CONSTANT_TK 1
#define EXPRESSION_OPERATOR_TK 2

/* Terminate symbols */
#define EXPRESSION_TERMINATE_OPERATORS ",+-*/=<>\0"
#define EXPRESSION_TERMINATE_BRACKETS  "|{}()[]\"\0"

/* Module parameters */
#define EXPRESSION_MAX_LEN 255

/* Expression exception messages */
#define EXPRESSION_UNDEFINED_OPERATOR "undefined operator"

/* The struct will contained the token of expression */
typedef struct ExpressionToken {
    char * literal;   // literal which define the tokens
    char type_id;     // the type of token
    void * value;      // the real value of token
    char vtype_id;    // the type of real value
} ExpressionToken;

/* The function will extract the expression tokens and return the list of it */
Array ** expression_lexer(Array ** tokens);

/* The function will extract the expression tokens from literal and store it to array */
Array ** extract_exp_token(char * literal);

/* The function will cut the constant from literal like numbers and variable name */
char * cut_constant(char * token);

/* The function will cut the operator from literal */
char * cut_operator(char * token);

/* The function will concatenating the literal of tokens and return a string */
char * cut_string(Array ** exp_tokens);

/* The function will store the tokens between {} and return it as array */
Array ** cut_array(Array ** exp_tokens);

/* The function will create real value of token in a heap */
void token_typecast(Array ** exp_tokens);

/* The function will allocate data of integer, float or string type into heap */
int allocate_token_value(ExpressionToken * exp_token);

/* The function will return 1, if the symbol in the stop_symbols array */
int is_in(char symbol, char * stop_symbols);

/* The function will return 1, if the literal containing only number */
int is_int_number(char * literal);

/* The function will return 1, if the litreal containing only number and dot-symbol */
int is_float_number(char * literal);

/* The function will return 1, if the literal is exactly variable name */
int is_name(char * literal);

/* Function will return the type of non-symbolic and non-numeric literal */
char get_token_type(char symbol);

/* Tools */
ExpressionToken * get_next_exp_token(Array ** exp_tokens);
ExpressionToken * pop_next_exp_token(Array ** exp_tokens);
ExpressionToken * get_curr_exp_token(Array ** exp_tokens);

/* Destructor */
void exp_token_destructor(ExpressionToken * token);
void exp_tokens_destructor(Array **);
#endif //VIPER_V4_EXPRESSION_H
