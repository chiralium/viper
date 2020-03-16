#include "expression.h"
#include "arithmetica.h"

Index * new_index(void * object, Array ** params, int params_count) {
    Index * index = malloc(sizeof(Index));
    index->object = object;
    index->params = params;
    index->params_count = params_count;
    return index;
}

void index_destructor(Index * index) {
    exp_token_destructor(index->object);
    array_destructor(index->params);
    free(index);
}

FuncCall * new_func_call(char * name, Array ** arg_list) {
    FuncCall * funccall = malloc(sizeof(FuncCall));
    char * function_name = alloc_string(name);
    funccall->name = function_name;
    funccall->arg_list = arg_list;
    return funccall;
}

void func_call_destructor(FuncCall * funccall) {
    array_destructor(funccall->arg_list);
    free(funccall->name);
    free(funccall);
}

void * assign_function(char * literal) {
    void * (*function_pointer)(void *, void *) = NULL;
    if (strcmp(literal, ARITHMETICA_PLUS) == 0) function_pointer = _add;
    else if (strcmp(literal, ARITHMETICA_SUB) == 0) function_pointer = _sub;
    else if (strcmp(literal, ARITHMETICA_MUL) == 0) function_pointer = _mul;
    else if (strcmp(literal, ARITHMETICA_DIV) == 0) function_pointer = _div;
    else if (strcmp(literal, ARITHMETICA_MORE) == 0) function_pointer = _more;
    else if (strcmp(literal, ARITHMETICA_LESS) == 0) function_pointer = _less;
    else if (strcmp(literal, ARITHMETICA_MEQ) == 0) function_pointer = _moreeq;
    else if (strcmp(literal, ARITHMETICA_LEQ) == 0) function_pointer = _lesseq;
    else if (strcmp(literal, ARITHMETICA_EQ) == 0) function_pointer = _equal;
    else if (strcmp(literal, ARITHMETICA_OCB) == 0 ||
             strcmp(literal, ARITHMETICA_CCB) == 0 ||
             strcmp(literal, ARITHMETICA_OBB) == 0 ||
             strcmp(literal, ARITHMETICA_CBB) == 0 ||
             strcmp(literal, ARITHMETICA_OSB) == 0 ||
             strcmp(literal, ARITHMETICA_CSB) == 0 ||
             strcmp(literal, ARITHMETICA_ASG) == 0 ||
             strcmp(literal, ARITHMETICA_COMA) == 0) function_pointer = _tmp; // this is only for the first time

    return function_pointer;
}

void * _add(void * x, void * y) {
    return NULL;
}

void * _sub(void * x, void * y) {
    return NULL;
}

void * _mul(void * x, void * y) {
    return NULL;
}

void * _div(void * x, void * y) {
    return NULL;
}

void * _more(void * x, void * y) {
    return NULL;
}

void * _less(void * x, void * y) {
    return NULL;
}

void * _moreeq(void * x, void * y) {
    return NULL;
}

void * _lesseq(void * x, void * y) {
    return NULL;
}

void * _equal(void * x, void * y) {
    return NULL;
}

void * _tmp(void * x, void * y) {
    return NULL;
}