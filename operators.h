/*
 * The header with define of the arithmetical operators
 */

#ifndef VIPER_V4_OPERATORS_H
#define VIPER_V4_OPERATORS_H

#define OP_PLUS  '+'
#define OP_MINUS '-'
#define OP_MUL   '*'
#define OP_DIV   '/'

#define OP_OPEN_CBRACK  '('
#define OP_CLOSE_CBRACK ')'
#define OP_OPEN_BBRACK  '{'
#define OP_CLOSE_BBRACK '}'
#define OP_OPEN_SBRACK  '['
#define OP_CLOSE_SBRACK ']'

#define OP_QUOTE '"'
#define OP_SPACE ' '
#define OP_COMA  ','
#define OP_ESCAPE '\\'

/* Define macro for operators */
#define OPERATOR_PLUS    0
#define OPERATOR_MINUS   1
#define OPERATOR_MUL     2
#define OPERATOR_DIV     3
#define OPERATOR_POW     4
#define OPERATOR_MORE    5
#define OPERATOR_LESS    6
#define OPERATOR_MEQ     7
#define OPERATOR_LEQ     8
#define OPERATOR_EQ      9
#define OPERATOR_NOT     10
#define OPERATOR_ASG     11
#define OPERATOR_ASC     12


#endif //VIPER_V4_OPERATORS_H
