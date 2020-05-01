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
#define EXPRESSION_CONSTANT_TK        1
#define EXPRESSION_OPERATOR_TK        2
#define EXPRESSION_CONSTANT_FUNC_TK   3
#define EXPRESSION_OPERATOR_EXT_TK    4

/* Terminate symbols */
#define EXPRESSION_TERMINATE_OPERATORS "!^+-*/=<>\\\0"
#define EXPRESSION_TERMINATE_BRACKETS  ",{}()[]\0"

/* Module parameters */
#define EXPRESSION_MAX_LEN 255

/* Expression exception messages */
#define EXPRESSION_UNDEFINED_TOKEN            "undefined token"
#define EXPRESSION_INVALID_INDEX_DECLARATION  "invalid index declaration"
#define EXPRESSION_TOO_MUCH_INDEX_PARAMS      "too much index parameters"
#define EXPRESSION_INVALID_ARRAY_DECLARATION  "invalid array declaration"
#define EXPRESSION_INVALID_ESCAPE_CHAR        "invalid escape character"
#define EXPRESSION_INVALID_FUNCTION_CALL      "invalid arguments definition (braces is not balanced)"
#define EXPRESSION_INVALID_FUNCTION_ARGLIST   "invalid arguments definition (empty arguments position after coma)"
#define EXPRESSION_INVALID_FUNCTION_NAME      "invalid function name"

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

/* The function will concatenating the all symbols between quote-marks */
char * cut_string(char * literal);

/* The function will return real character that assignment with escaped character */
char escape2real(char symbol);

/* The function will return 1, if the symbol in the stop_symbols array */
int is_in(char symbol, char * stop_symbols);

/* The function will return 1, if the literal is exactly variable name */
int is_name(char * literal);

/* The function will return 1, if the current open circle brack is a function call */
int is_function_call(char prev_token_type, char current_bracket);

/* Function will return the type of non-symbolic and non-numeric literal */
char get_token_type(char symbol);

/* Function will convert the token array to string */
char * as_string(Array ** exp_token);

/* Destructor */
void exp_token_destructor(ExpressionToken * token);

/* Copy */
ExpressionToken * copy_expression_token(ExpressionToken * token);
#endif //VIPER_V4_EXPRESSION_H
