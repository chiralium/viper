#include <windows.h>
#include "exception.h"
#include "interpreter.h"
#include "display.h"

extern Array ** call_stack;

void throw_internal_error(char * broken_line) {
    display_callstack(call_stack);
    set_color_scheme(COLOR_SCHEME_EXCEPTION);
    printf(EXCEPTION_INTERNAL_ERROR, broken_line);
    set_color_scheme(-1);
    exit(0);
}

void throw_composer_exception(char * broken_token, char * message) {
    display_callstack(call_stack);
    set_color_scheme(COLOR_SCHEME_EXCEPTION);
    printf(EXCEPTION_COMPOSER_ERROR, broken_token, message);
    set_color_scheme(-1);
    exit(0);
}

void throw_function_exception(char * broken_line, char * message, char * function_name) {
    display_callstack(call_stack);
    set_color_scheme(COLOR_SCHEME_EXCEPTION);
    char message_with_function_name[512];
    sprintf(message_with_function_name, message, function_name);

    printf(EXCEPTION_FUNCTION_CALL_ERROR, broken_line, message_with_function_name);
    set_color_scheme(-1);
    exit(0);
}

void throw_code_structure_exception(int line, char * message) {
    display_callstack(call_stack);
    set_color_scheme(COLOR_SCHEME_EXCEPTION);
    printf(EXCEPTION_CODE_STRUCTURE_ERROR, line, message);
    set_color_scheme(-1);
    exit(0);
}

void throw_statement_exception(char * broken_line, char * message) {
    display_callstack(call_stack);
    set_color_scheme(COLOR_SCHEME_EXCEPTION);
    printf(EXCEPTION_STATEMENT_ERROR, broken_line, message);
    set_color_scheme(-1);
    exit(0);
}

void throw_arithmetical_exception(char * broken_line, char * message) {
    display_callstack(call_stack);
    set_color_scheme(COLOR_SCHEME_EXCEPTION);
    printf(EXCEPTION_ARITHMETICAL_ERROR, broken_line, message);
    set_color_scheme(-1);
    exit(0);
}

void throw_typecasting_exception(char * broken_line, char * message) {
    display_callstack(call_stack);
    set_color_scheme(COLOR_SCHEME_EXCEPTION);
    printf(EXCEPTION_TYPECASTING_ERROR, broken_line, message);
    set_color_scheme(-1);
    exit(0);
}

void throw_warning_message(char * dangerous_line, char * message) {
    display_callstack(call_stack);
    set_color_scheme(COLOR_SCHEME_WARNING);
    printf(EXCEPTION_WARNING, dangerous_line, message);
    set_color_scheme(-1);
}