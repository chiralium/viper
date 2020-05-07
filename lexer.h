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
#define LEXER_KEYWORD_TK       1
#define LEXER_KEYWORD_PARAM_TK 4
#define LEXER_EXPRESSION_TK    2
#define LEXER_COMPLEX_TK       3
#define LEXER_IF_TK            5
#define LEXER_ELSE_TK          6
#define LEXER_FOR_TK           7
#define LEXER_FUNCTION_TK      8
#define LEXER_WHILE_TK         9
#define LEXER_RETURN_TK       10
#define LEXER_NAMESPACE_TK    11
#define LEXER_GLOBAL_TK       12

/* Lexer parameters */
#define LEXER_MAX_VALUE  1024

/*
 * The structure Token having type_id -- the define and value -- the literals
 */
typedef struct Token {
    char type_id;
    void * value;
} Token;

/* The main lexer function */
Array ** lexer(Array ** literals);

/* The function will extracting a token from a string and store it in the array of tokens */
Array ** extract_token(char * literal, Array ** tokens);

/* The function will cut the substring while not meet the stop symbol */
char * cut_token(char * literal, char stop_symbol);

/* The function return 1 if the symbol is special symbols as +, -, ", etc. */
int is_special(char symbol);

/* The function return the token id if the literal is keyword, else return 0 */
int is_keyword(char * literal);

/* Free the memory of token */
void token_destructor(Token * token);

/* Getting the first symbol from literal */
char get_first(char * literal);

/* Getting the last symbol from literal */
char get_last(char * literal);

#endif //VIPER_V4_LEXER_H
