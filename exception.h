#include <stdio.h>
#include <stdlib.h>
#include "array.h"

/*
 * The functions which terminate the program with specific exceptions
 */

#ifndef VIPER_V4_EXCEPTION_H
#define VIPER_V4_EXCEPTION_H

#define EXCEPTION_CODE_STRUCTURE_ERROR "-> StructureError: at line %d (%s)\n"
#define EXCEPTION_STATEMENT_ERROR      "-> StatementError: at line `%s` (%s)\n"
#define EXCEPTION_ARITHMETICAL_ERROR   "-> ArithmeticalError: at line `%s` (%s)\n"
#define EXCEPTION_TYPECASTING_ERROR    "-> TypeError: at line `%s` (%s)\n"
#define EXCEPTION_FUNCTION_CALL_ERROR  "-> FunctionCallError: at line `%s` (%s)\n"
#define EXCEPTION_INTERNAL_ERROR       "-> SystemError: at line `%s`"

/* Something gonna be really wrong. What is happening? */
void throw_internal_error(char * broken_line);

/* Getting the line and terminate a program with EXCEPTION_FUNCTION_CALL_ERROR at line */
void throw_function_exception(char * broken_line, char * message, char * function_name);

/* Getting the line and terminate a program with EXCEPTION_CODE_STRUCTURE_ERROR at line */
void throw_code_structure_exception(int line, char * message);

/* Getting the broken statement and terminate the program with EXCEPTION_STATEMENT_ERROR */
void throw_statement_exception(char * broken_line, char * message);

/* Getting the broken expression and terminate the program with EXCEPTION_ARITHMETICAL_ERROR */
void throw_arithmetical_exception(char * broken_line, char * message);

/* Getting the broken expression and terminate the program with EXCEPTION_TYPECASTING_ERROR */
void throw_typecasting_exception(char * broken_line, char * message);

#endif //VIPER_V4_EXCEPTION_H
