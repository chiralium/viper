#include <stdio.h>
#include <stdlib.h>
/*
 * The functions which terminate the program with specific exceptions
 */

#ifndef VIPER_V4_EXCEPTION_H
#define VIPER_V4_EXCEPTION_H

#define EXCEPTION_CODE_STRUCTURE_ERROR "StructureError: at line %d (%s)"
#define EXCEPTION_STATEMENT_ERROR      "StatementError: at line %s (%s)"

/* Getting the line and terminate a program with EXCEPTION_CODE_STRUCTURE_ERROR at line */
void throw_code_structure_exception(int line, char * message);

/* Getting the broken statement and terminate a program with EXCEPTION_STATEMENT_ERROR */
void throw_statement_exception(char * broken_line, char * message);

#endif //VIPER_V4_EXCEPTION_H
