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
    Array ** else_body;
} If;

/* The structure For will contained the information about the for-statement */
typedef struct For {
    char * params;
    Array ** body;
} For;

/* The structure Expression will contained the information about the expression-statement */
typedef struct Expression {
    char * expr;
    char type_id;
    void * value;
} Expression;

/* The function will parse the input token list and return the array with structures */
Array ** parser(Array ** tokens);

/* The function will create the new structure of Function */
Function * make_function(char * name, Array ** arg_list, Array ** body);

/* The function will create the new structure of If */
If * make_if(char * condition, Array ** body, Array ** else_body);

/* The function will create the new structure of For */
For * make_for(char * params, Array ** body);

/* Extract the arguments of function from literal */
Array ** extract_arguments(char * literal);

/* Extract the name of function from literal */
char * extract_name(char * literal);

/* The function will be cut a sub-array of tokens */
Array ** cut_tokens(Array ** tokens, int stop_id);

/* The function will cut the space from start and end of the literal */
char * trim(char * literal);

#endif //VIPER_V4_PARSER_H
