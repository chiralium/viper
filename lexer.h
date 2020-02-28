#include "array.h"
#include "operators.h"

/*
 * The tools for tokenize the array of literals from fread-module
 */
#ifndef VIPER_V4_LEXER_H
#define VIPER_V4_LEXER_H

/* Token types */
#define LEXER_KEYWORD_TK    1
#define LEXER_EXPRESSION_TK 2

/* Lexer parameters */
#define LEXER_MAX_TOKENS 100
#define LEXER_MAX_VALUE  255

/*
 * The structure Token having type_id -- the define and value -- the literals
 */
typedef struct Token {
    char type_id;
    char * value;
} Token;

/* The main lexer function */
Array ** lexer(Array ** literals);

/* The function cut_keyword divide the string by tokens at LEXER_KEYWORD_TK and LEXER_EXPRESSION_TK */
char * cut_keyword(char * literal);

#endif //VIPER_V4_LEXER_H
