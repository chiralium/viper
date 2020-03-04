#include "lexer.h"
#include "exception.h"
#include "array.h"
#include "operators.h"
#include "keywords.h"

/*
 * The tools for parsing the token-list from lexer-module and create special structures for keyword-tokens.
 * This is the high-level parser
 */
#ifndef VIPER_V4_PARSER_H
#define VIPER_V4_PARSER_H

/* The message about problem with token parsing */
#define PARSER_MISSING_ELSE_BODY_MSG "the body of `else` statement not defined"
#define PARSER_MISSING_IF_BODY_MSG   "the body of `if` statement not defined"
#define PARSER_MISSING_IF_CONDITION  "the expression of `if` statement not defined"

/* The structure function will contained the information of function. Then this structure stored in NameSpace */
typedef struct Function {
    char * name;
    Array ** arg_list;
    Array ** body;
} Function;

/* The structure If will contained the information about the if-statement */
typedef struct If {
    char * condition;
    Array ** body;
    char * else_condition;
    Array ** else_body;
} If;

/* The structure For will contained the information about the for-statement */
typedef struct For {
    char * params;
    Array ** body;
} For;

/* The function will parse the input token list and return the array with structures */
Array ** parser(Array ** tokens);

/* The function will create the new structure of If */
If * make_if(char * condition, Array ** body, char * else_condition, Array ** else_body);

/* The function will collect the elements of if-statement */
If * get_if_statement(Array ** tokens);

/* The function will create the new structure of Function */
Function * make_function(char * name, Array ** arg_list, Array ** body);

/* The function will create the new structure of For */
For * make_for(char * params, Array ** body);

/* Return the next tokens from token list */
Token * next_token(Array ** tokens);

/* The function will cut the space from start and end of the literal */
char * trim(char * literal);

/* Destructors */
void if_destructor(If * statement);
void for_destructor(For * statement);
void function_destructor(Function * statement);

#endif //VIPER_V4_PARSER_H
