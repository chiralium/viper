#include "exception.h"

void throw_internal_error(char * broken_line) {
    printf(EXCEPTION_INTERNAL_ERROR, broken_line);
    exit(0);
}

void throw_function_exception(char * broken_line, char * message, char * function_name) {
    char message_with_function_name[512];
    sprintf(message_with_function_name, message, function_name);
    printf(EXCEPTION_FUNCTION_CALL_ERROR, broken_line, message_with_function_name);
    exit(0);
}

void throw_code_structure_exception(int line, char * message) {
    printf(EXCEPTION_CODE_STRUCTURE_ERROR, line, message);
    exit(0);
}

void throw_statement_exception(char * broken_line, char * message) {
    printf(EXCEPTION_STATEMENT_ERROR, broken_line, message);
    exit(0);
}

void throw_arithmetical_exception(char * broken_line, char * message) {
    printf(EXCEPTION_ARITHMETICAL_ERROR, broken_line, message);
    exit(0);
}

void throw_typecasting_exception(char * broken_line, char * message) {
    printf(EXCEPTION_TYPECASTING_ERROR, broken_line, message);
    exit(0);
}