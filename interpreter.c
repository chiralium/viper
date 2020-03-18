#include "interpreter.h"

void interpreter(Array ** code) {
    int code_counter = 0;
    while (code[code_counter]) {
        if (code[code_counter]->type_id == ARRAY) {
            // if this condition is true, then this element is a expression
            Constant * value = calculate_expression(code[code_counter]->element);
            display_constant(value);
            constant_destructor(value);
            free(code[code_counter]);
        } else if (code[code_counter]->type_id == STMT_IF) {
            if_destructor(code[code_counter]->element);
            free(code[code_counter]);
        } else if (code[code_counter]->type_id == STMT_WHILE) {
            free(code[code_counter]->element);
            free(code[code_counter]);
        } else if (code[code_counter]->type_id == STMT_FUNC) {
            function_destructor(code[code_counter]->element);
            free(code[code_counter]);
        }
        code_counter++;
    }
    free(code);
}

Constant * calculate_expression(Array ** expression) {
    Constant * value = arithmetica(expression);
    return value;
}