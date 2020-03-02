#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exception.h"
#include "array.h"

/*
 * The tools for reading file and separate it by main structure.
 */

#ifndef VIPER_V4_FREAD_H
#define VIPER_V4_FREAD_H

/* Delimiters for main logical structure of program code */
#define FPARSER_NODE_DELIMITER           ';'
#define FPARSER_COMPLEX_DELIMITER        '{'
#define FPARSER_COMPLEX_DELIMITER_CLOSE  '}'
#define FPARSER_NODE_DELIMITER_2         ':'
#define FPARSER_QUOTE                    '"'

/* Special constants */
#define FPARSER_MAX_STRUCT_LEN           255
#define FPARSER_EOL                      10
#define FPARSER_SPACE                    32

/* The fread-module message about some problems with parsing */
#define FPARSER_QUOTE_BALANCED_MSG "quote mark(s) is not balanced"
#define FPRASE_STRUCT_BALANCED_MSG "structure identifier '{}' is not balanced"
#define FPRASE_STRUCT_IS_EMPTY_MSG "the structure of code should be not empty"


/* Getting the size of file in bytes */
unsigned long get_size(FILE * f);

/* Reading file and return the big char array */
char * readfile(char * filename);

/* Extracting structure by delimiter */
char * cut_structure_node(char * input_stream);

/* Extracting structure from complex structure like {...} */
char * cut_complex_structure(char * input_stream);

/* The recursive descent method for analyzing code structure */
Array ** recursive_descent(char * input_stream);

/* Allocated the memory for structure */
char * alloc_string(char * structure);

/* Pop up the first element of input_stream */
char pop_first(char * input_stream);

/* Pop up the last element of input_stream */
char pop_last(char * input_stream);

/* The symbols is ignored? */
int is_ignored(char symbol);

#endif //VIPER_V4_FREAD_H
