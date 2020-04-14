#include "composer.h"

static int _next_token;
char * tokens_like_string; // the token list as string for exceptions

Array ** composer(Array ** token_list) {
    /* Each line that separated by semicolon is an single token list */
    int counter = 0; Array ** objects = NULL;
    while (token_list[counter]) {
        Array ** list = token_list[counter]->element;
        objects = object_assembler(list);
        counter++;
    }
    return objects;
}

Array ** object_assembler(Array ** token_list) {
    tokens_like_string = as_string(token_list);
    Array ** objects = new_array();
    int counter = 0;
    while (token_list[counter]) {
        ExpressionToken * token = token_list[counter]->element;
        if (token->type_id == OP_OPEN_BBRACK) {
            /* {...} */
        } else if (token->type_id == OP_OPEN_SBRACK) {
            /* <exp>[...] */
        } else if (token->type_id == EXPRESSION_CONSTANT_FUNC_TK) {
            /* foo(x1, x2, ..., xN) */
        } else {
            Constant * simple_object = compose_simple_data(token);
            objects = append(objects, OBJECT, simple_object);
        }
        counter++;
    }
    return objects;
}

Constant * compose_simple_data(ExpressionToken * token) {
    char * token_literal = token->literal;
    void * (*operator)(void *, void *) = NULL;
    Constant * composed = NULL;

    if (token->type_id == EXPRESSION_CONSTANT_STR_TK) {
        char * value = alloc_string(token->literal);
        composed = new_constant(COMPOSER_OBJECT_STRING, value);
    } else if (is_int_number(token_literal)) {
        int * value = malloc(sizeof(int)); *value = atoi(token_literal);
        composed = new_constant(COMPOSER_OBJECT_INT, value);
    } else if (is_float_number(token_literal)) {
        float *  value = malloc(sizeof(float)); *value = atof(token_literal);
        composed = new_constant(COMPOSER_OBJECT_FLOAT, value);
    } else if (operator = assign_function(token_literal)) {
        if (operator != _tmp) composed = new_constant(COMPOSER_OBJECT_OPERATOR, operator);
        else composed = new_constant(COMPOSER_OBJECT_MATH, token->type_id);
    } else if (is_name(token_literal)) {
        char * variable_name = alloc_string(token_literal);
        composed = new_constant(COMPOSER_OBJECT_VARIABLE, variable_name);
    } else throw_composer_exception(token_literal, COMPOSER_UNDEFINED_TOKEN);
    return composed;
}

char * get_operator_as_string(void * operator_pointer) {
    if (operator_pointer == _add) {
        return COMPOSER_OPERATOR_PLUS;
    } else if (operator_pointer == _sub) {
        return COMPOSER_OPERATOR_SUB;
    } else if (operator_pointer == _mul) {
        return COMPOSER_OPERATOR_MUL;
    } else if (operator_pointer == _div) {
        return COMPOSER_OPERATOR_DIV;
    } else if (operator_pointer == _pow) {
        return COMPOSER_OPERATOR_POW;
    } else if (operator_pointer == _equal) {
        return COMPOSER_OPERATOR_EQ;
    } else if (operator_pointer == _more) {
        return COMPOSER_OPERATOR_MORE;
    } else if (operator_pointer == _less) {
        return COMPOSER_OPERATOR_LESS;
    } else if (operator_pointer == _moreeq) {
        return COMPOSER_OPERATOR_MEQ;
    } else if (operator_pointer == _lesseq) {
        return COMPOSER_OPERATOR_LEQ;
    } else if (operator_pointer == _asg) {
        return COMPOSER_OPERATOR_ASG;
    } else if (operator_pointer == _asc) {
        return COMPOSER_OPERATOR_ASC;
    }
}