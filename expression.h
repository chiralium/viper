#include "fread.h"
#include "parser.h"
#include "operators.h"
#include "array.h"
#include "exception.h"

/*
 * Tools for parsing the type 2 tokens. Return a list of expression tokens
 */
#ifndef VIPER_V4_EXPRESSION_H
#define VIPER_V4_EXPRESSION_H

/* Tokens type */
#define EXPRESSION_CONSTANT_TK 1
#define EXPRESSION_OPERATOR_TK 2
#define EXPRESSION_BRACKET_TK  3

/* Terminate symbols */
#define EXPRESSION_TERMINATE_OPERATORS "+-*/=<>\0"
#define EXPRESSION_TERMINATE_BRACKETS  "()[]\"\0"

/* Module parameters */
#define EXPRESSION_MAX_LEN 255

/* The struct will contained the token of expression */
typedef struct ExpressionToken {
    char * literal;   // literal which define the tokens
    char type_id;     // the type of token
} ExpressionToken;

/* The function will extract the expression tokens and return the list of it */
Array ** expression_lexer(Array ** tokens);

/* The function will extract the expression tokens from literal and store it to array */
Array ** extract_exp_token(char * literal);

/* The function will cut the constant from literal like numbers and variable name */
char * cut_constant(char * token);

/* The function will cut the operator from literal */
char * cut_operator(char * token);

/* The function will return 1, if the symbol in the stop_symbols array */
int is_in(char symbol, char * stop_symbols);

/* Destructor */
void exp_token_destructor(ExpressionToken * token);
void exp_tokens_destructor(Array ** );
#endif //VIPER_V4_EXPRESSION_H
