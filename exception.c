#include "exception.h"

void throw_internal_error(char * broken_line) {
    printf(EXCEPTION_INTERNAL_ERROR, broken_line);
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