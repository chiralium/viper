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
#define EXPRESSION_CONSTANT_TK       1
#define EXPRESSION_OPERATOR_TK       2
#define EXPRESSION_CONSTANT_FUNC_TK  3

/* Terminate symbols */
#define EXPRESSION_TERMINATE_OPERATORS "!^+-*/=<>\\\0"
#define EXPRESSION_TERMINATE_BRACKETS  ",{}()[]\0"

/* Module parameters */
#define EXPRESSION_MAX_LEN 255

/* Expression exception messages */
#define EXPRESSION_UNDEFINED_TOKEN             "undefined token"
#define EXPRESSION_INVALID_INDEX_DECLARATION  "invalid index declaration"
#define EXPRESSION_TOO_MUCH_INDEX_PARAMS      "too much index parameters"
#define EXPRESSION_INVALID_ARRAY_DECLARATION  "invalid array declaration"
#define EXPRESSION_INVALID_ESCAPE_CHAR        "invalid escape character"
#define EXPRESSION_INVALID_FUNCTION_CALL      "invalid arguments definition (braces is not balanced)"

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



/* ROLLBACK SECTION ---------------------------------------------------------- */
/* The function will cut the tokens while not meet the stop token */
Array ** cut_index_body(Array ** exp_tokens);

/* The function will cut the index object */
Array ** cut_index_object(Array ** exp_tokens, int * position);

/* The function will cut the tokens between [] */
Array ** cut_index_el(Array ** exp_tokens);

/* The function will store the tokens between [] and return it as array of tokens */
Array ** cut_index(Array ** exp_tokens);

/* The function will store the tokens between () while not meet the coma */
Array ** cut_single_arg(Array ** exp_tokens);

/* The function will store the tokens between () like array of array */
Array ** cut_arglist(Array ** exp_tokens);

/* The function will create real value of token in a heap just for simple data*/
void typecast_constant(Array ** exp_tokens);

/* The function will compose the arrays from tokens stream */
void typecast_array(Array ** exp_tokens);

/* The function will compose the index from tokens stream */
void typecast_index(Array ** exp_tokens);

/* The function will compose the function call structure */
void typecast_function(Array ** exp_tokens);
/* ROLLBACK SECTION ---------------------------------------------------------- */


/* The function will typecasting token list to data structures */
void token_typecasting(Array ** exp_tokens);

/* The function will typecasting token list to array-structure */
Array ** array_typecasting(Array ** exp_tokens, int position);

/* The function will be cut parrent array */
Array ** cut_array_body(Array ** exp_tokens, int position);

/* The function will be collect array elements like token list */
Array ** cut_array_element(Array ** exp_tokens);

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

/* Function will return the operator type */
char get_operator_type(char * operator);

/* Function will convert the token array to string */
char * as_string(Array ** exp_token);

/* Tools */
ExpressionToken * get_next_exp_token(Array ** exp_tokens);
ExpressionToken * pop_next_exp_token(Array ** exp_tokens);
ExpressionToken * pop_exp_token(Array ** exp_tokens, int position);
ExpressionToken * get_curr_exp_token(Array ** exp_tokens);

/* Destructor */
void exp_token_destructor(ExpressionToken * token);
void exp_tokens_destructor(Array **);
#endif //VIPER_V4_EXPRESSION_H
