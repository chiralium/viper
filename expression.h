#include "fread.h"
#include "operators.h"

/*
 * Tools for parsing the type 2 tokens. Return a list of expression tokens
 */
#ifndef VIPER_V4_EXPRESSION_H
#define VIPER_V4_EXPRESSION_H

/* Tokens type */
#define EXPRESSION_CONSTANT_TK 1
#define EXPRESSION_OPERATOR_TK 2

/* Terminate symbols */
#define EXPRESSION_TERMINATE_OP "()[]+-*/=<>\"\0"
#define EXPRESSION_TERMINATE_C  "0123456789\0"

/* Module parameters */
#define EXPRESSION_MAX_LEN 255

/* The struct will contained the tokens of expression */
typedef struct ExpressionToken {
    char * literal;   // literal which define the tokens
    char type_id;     // the type of token
} ExpressionToken;

/* The function will cut the token from literal while the current symbol is not in the stop_symbols array */
char * smart_cut(char * token, char * stop_symbols);

/* The function will return 1, if the symbol in the stop_symbols array */
int is_in(char symbol, char * stop_symbols);

#endif //VIPER_V4_EXPRESSION_H
