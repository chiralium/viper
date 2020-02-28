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
char * alloc_structure(char * structure);

/* Pop up the first element of input_stream */
char pop_up(char * input_stream);

#endif //VIPER_V4_FREAD_H
