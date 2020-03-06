#include "expression.h"

int is_in(char symbol, char * stop_symbols) {
    int counter = 0;
    while (stop_symbols[counter] != symbol && stop_symbols[counter]) counter++;
    return !stop_symbols[counter] ? 0 : 1;
}

char * smart_cut(char * token, char * stop_symbols) {
    char stack_tmp[EXPRESSION_MAX_LEN]; int tmp_counter = 0;
    char symbol;
    while (symbol = pop_first(token)) {
        if (!is_in(symbol, stop_symbols)) stack_tmp[tmp_counter++] = symbol;
        else break;
    }
    stack_tmp[tmp_counter] = '\0';
    char * literal = alloc_string(stack_tmp);
    return literal;
}