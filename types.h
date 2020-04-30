#ifndef VIPER_V4_TYPES_H
#define VIPER_V4_TYPES_H

/* Viper's types */
#define INTEGER      'i'
#define FLOAT        'f'
#define STRING       's'
#define VIARRAY       8
#define KEYPAIR       9
#define FUNCTION     11
#define NONE         12
#define NAMESPACE    13

/* Basic information types */
#define INTEGER      'i'
#define FLOAT        'f'
#define ARRAY        'a'
#define STRING       's'
#define UNDEFINED    'u'
#define SYS          '-'
#define OPERATOR     '1'
#define INDEX        '2'
#define FUNCTION_RES '3'

/* Entire system types */
#define TOKEN                't'
#define NULL_TOKEN           'n'
#define EXP_TK                5
#define COMPLEX_TOKEN        'c'
#define STMT_IF               1
#define STMT_FUNC             2
#define STMT_WHILE            3
#define STMT_RETURN          10
#define STMT_NAMESPACE       15
#define ARRAY_EL             'e'
#define ELEMENT               6
#define CONSTANT              7
#define MEMORY_ELEMENT       13
#define FUNCTION_CONTAINER   14

#endif //VIPER_V4_TYPES_H
