#include <windows.h>
#include "exception.h"
#include "interpreter.h"

extern Array ** call_stack;

void throw_internal_error(char * broken_line) {
    display_callstack(call_stack);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    printf(EXCEPTION_INTERNAL_ERROR, broken_line);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    exit(0);
}

void throw_composer_exception(char * broken_token, char * message) {
    display_callstack(call_stack);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    printf(EXCEPTION_COMPOSER_ERROR, broken_token, message);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    exit(0);
}

void throw_function_exception(char * broken_line, char * message, char * function_name) {
    display_callstack(call_stack);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    char message_with_function_name[512];
    sprintf(message_with_function_name, message, function_name);

    printf(EXCEPTION_FUNCTION_CALL_ERROR, broken_line, message_with_function_name);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    exit(0);
}

void throw_code_structure_exception(int line, char * message) {
    display_callstack(call_stack);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    printf(EXCEPTION_CODE_STRUCTURE_ERROR, line, message);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    exit(0);
}

void throw_statement_exception(char * broken_line, char * message) {
    display_callstack(call_stack);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    printf(EXCEPTION_STATEMENT_ERROR, broken_line, message);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    exit(0);
}

void throw_arithmetical_exception(char * broken_line, char * message) {
    display_callstack(call_stack);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    printf(EXCEPTION_ARITHMETICAL_ERROR, broken_line, message);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    exit(0);
}

void throw_typecasting_exception(char * broken_line, char * message) {
    display_callstack(call_stack);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    printf(EXCEPTION_TYPECASTING_ERROR, broken_line, message);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    exit(0);
}

void throw_warning_message(char * dangerous_line, char * message) {
    display_callstack(call_stack);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE);
    printf(EXCEPTION_WARNING, dangerous_line, message);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}