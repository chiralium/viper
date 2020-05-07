#include "array.h"
#include "expression.h"

/*
 * The module that compose the token list into structure like arrays, indexes and function call
 */

#ifndef VIPER_V4_COMPOSER_H
#define VIPER_V4_COMPOSER_H

/* Main entry point into composer */
void composer(Array ** token_list);

/* The function will be cut the function pointer expression */
Array ** cut_function_pointer(Array ** exp_tokens, int * position);

/* The function will be cut arg-list of function */
Array ** cut_function_arglist(Array ** exp_tokens, int position);

/* The function will be cut arg-list body of function */
Array ** cut_arglist_body(Array ** exp_tokens, int position);

/* The function will be cut single argument from arg-list */
Array ** cut_argument(Array ** exp_tokens);

/* The function will typecasting token list to array-structure */
Array ** array_typecasting(Array ** exp_tokens, int position);

/* The function will be cut parent array */
Array ** cut_array_body(Array ** exp_tokens, int position);

/* The function will be collect array elements like token list */
Array ** cut_array_element(Array ** exp_tokens);

/* The function will typecasting token list to index-structure */
Array ** index_typecasting(Array ** exp_tokens, int position);

/* The function will be cut parent index body */
Array ** cut_index_body(Array ** exp_tokens, int position);

/* The function will be cut index parameter */
Array ** cut_index_parameter(Array ** exp_tokens);

/* The function will store the tokens of index's object */
Array ** cut_index_object(Array ** exp_tokens, int * position);

/* The function will typecasting token list to data structures */
void token_composer(Array ** exp_tokens);

/* The function will allocate data of integer, float or string type into heap */
int allocate_token_value(ExpressionToken * exp_token);

/* The function will return 1, if the literal containing only number */
int is_int_number(char * literal);

/* The function will return 1, if the literal containing only number and dot-symbol */
int is_float_number(char * literal);

/* Function will return the operator type */
char get_operator_type(char * operator);

/* Tools */
ExpressionToken * get_next_exp_token(Array ** exp_tokens);
ExpressionToken * pop_next_exp_token(Array ** exp_tokens);
ExpressionToken * pop_exp_token(Array ** exp_tokens, int position);
ExpressionToken * get_curr_exp_token(Array ** exp_tokens);

#endif //VIPER_V4_COMPOSER_H
