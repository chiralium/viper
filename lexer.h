#include <ctype.h>
#include "fread.h"
#include "array.h"
#include "operators.h"
#include "keywords.h"

/*
 * The tools for tokenize the array of literals from fread-module
 */
#ifndef VIPER_V4_LEXER_H
#define VIPER_V4_LEXER_H

/* Token types */
#define LEXER_KEYWORD_TK    1
#define LEXER_EXPRESSION_TK 2
#define LEXER_COMPLEX_TK    3

/* Lexer parameters */
#define LEXER_MAX_TOKENS 100
#define LEXER_MAX_VALUE  255

/*
 * The structure Token having type_id -- the define and value -- the literals
 */
typedef struct Token {
    char type_id;
    void * value;
} Token;

/* The main lexer function */
Array ** lexer(Array ** literals);

/* The function cut_keyword divide the string by tokens at LEXER_KEYWORD_TK and LEXER_EXPRESSION_TK */
char * cut_keyword(char * literal);

/* The function will cutting the expression token */
char * cut_expression(char * literal);

/* The function return 1 if the symbol is special symbols as +, -, ", etc. */
int is_special(char symbol);

/* The function return 1 if the literal is keyword */
int is_keyword(char * literal);

#endif //VIPER_V4_LEXER_H
