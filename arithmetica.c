#include "expression.h"
#include "arithmetica.h"

Node * namespace;

Constant * arithmetica(Array ** expression_tokens, Node * current_namespace) {
    namespace = current_namespace;
    char * expression_as_string = as_string(expression_tokens);
    Array ** postfixed_expression = postfix(expression_tokens);

    /*
     * Calculate the value:
     * 1. if current token is a complex value like array or index:
     *    - for array necessary precalculate each elements of array;
     *    - for index necessary precalculate index value;
     *    - save the precalculated value into value of ExpressionTokens
     *
     * 2. if current value is a variable:
     *    - get real value from namespace
     *    - save this value into value of ExpressionTokens
     *
     * 3. Calculate value and return as Constant-structure
     */

    Array ** constant_stack = new_array();
    int counter = 0;
    while (postfixed_expression[counter]) {
        ExpressionToken * token = postfixed_expression[counter]->element;
        if (token->type_id == EXPRESSION_CONSTANT_TK) constant_stack = append(constant_stack, EXP_TK, token);
        else {
            Array * stack_el_x = pop_last_el(constant_stack); Array * stack_el_y = pop_last_el(constant_stack);
            if (!stack_el_x || !stack_el_y) throw_arithmetical_exception(expression_as_string, ARITHMETICA_INVALID_EXPRESSION_SYNTAX);

            ExpressionToken * x_tk = stack_el_x->element; free(stack_el_x);
            ExpressionToken * y_tk = stack_el_y->element; free(stack_el_y);
            if (x_tk->type_id != EXPRESSION_CONSTANT_TK || y_tk->type_id != EXPRESSION_CONSTANT_TK) throw_arithmetical_exception(expression_as_string, ARITHMETICA_INVALID_OPERAND);

            void * (*function_pointer)(void *, void *) = token->value;
            ExpressionToken * result_tk = function_pointer(x_tk, y_tk);
            constant_stack = append(constant_stack, EXP_TK, result_tk);
        }
        counter++;
    }

    Array * last = pop_last_el(constant_stack);
    ExpressionToken * result_tk = last->element;

    Constant * value = malloc(sizeof(Constant));
    value->type_id = result_tk->vtype_id;
    value->value = copy_data(result_tk->value, result_tk->vtype_id); // copy data from to token, cause all token-list must be destroyed

    free(last);
    free(constant_stack);
    free(expression_as_string);
    array_destructor(postfixed_expression);
    return value;
}

Array ** postfix(Array ** expression_tokens) {
    char * expression = as_string(expression_tokens);

    expression_tokens = fixing_unary_operators(expression_tokens);

    Array ** tokens_stack = new_array(); Array ** output = new_array();
    Array * current_token;
    while (current_token = pop_el(expression_tokens)) {
        ExpressionToken * token = current_token->element;
        if (token->type_id == EXPRESSION_CONSTANT_TK) {
            output = append(output, EXP_TK, token);
            free(current_token);
        } else if (token->type_id == EXPRESSION_CONSTANT_FUNC_TK) {
            tokens_stack = append(tokens_stack, EXP_TK, token);
            free(current_token);
        } else if (token->type_id == OP_OPEN_CBRACK) {
            tokens_stack = append(tokens_stack, EXP_TK, token);
            free(current_token);
        } else if (token->type_id == OP_CLOSE_CBRACK) {
            Array * stack_el;
            while (stack_el = get_last_el(tokens_stack)) {
                ExpressionToken * stack_tk = stack_el->element;
                if (stack_tk->type_id != OP_OPEN_CBRACK) {
                    output = append(output, EXP_TK, pop_last_el(tokens_stack)->element);
                    free(stack_el);
                } else {
                    Array * stack_el = pop_last_el(tokens_stack);
                    exp_token_destructor(stack_el->element);
                    free(stack_el);
                    break;
                }
            }
            exp_token_destructor(current_token->element);
            free(current_token);
        } else if (token->type_id == EXPRESSION_OPERATOR_TK) {
            Array * stack_el;
            while (stack_el = get_last_el(tokens_stack)) {
                ExpressionToken * stack_tk = stack_el->element;
                if (_get_priority(stack_tk->literal) >= _get_priority(token->literal)) {
                    output = append(output, EXP_TK, pop_last_el(tokens_stack)->element);
                    free(stack_el);
                } else break;
            }
            tokens_stack = append(tokens_stack, EXP_TK, token); free(current_token);
        }
    }
    Array * stack_el;
    while (stack_el = pop_last_el(tokens_stack)) {
        ExpressionToken * token = stack_el->element;
        if (token->type_id == OP_OPEN_CBRACK || token->type_id == OP_CLOSE_CBRACK) throw_arithmetical_exception(expression, ARITHMETICA_BRACES_NOT_BALANCED);
        output = append(output, EXP_TK, token); free(stack_el);
    }

    free(expression); free(tokens_stack); free(expression_tokens);

    return output;
}

Array ** fixing_unary_operators(Array ** expression_tokens) {
    int token_counter = 0;
    while (expression_tokens[token_counter]) {
        ExpressionToken * token = expression_tokens[token_counter]->element;
        if (token->vtype_id == OPERATOR_PLUS || token->vtype_id == OPERATOR_MINUS) {
            if (!token_counter) expression_tokens = insert(expression_tokens, EXP_TK, make_zero_tk(), token_counter++);
            else if ( ((ExpressionToken *)(expression_tokens[token_counter - 1]->element))->type_id == OP_OPEN_CBRACK ||
                      ((ExpressionToken *)(expression_tokens[token_counter - 1]->element))->type_id == EXPRESSION_OPERATOR_TK) expression_tokens = insert(expression_tokens,
                                                                                                                                                          EXP_TK,
                                                                                                                                                          make_zero_tk(),
                                                                                                                                                          token_counter++);
        }
        token_counter++;
    }
    return expression_tokens;
}

int _get_priority(char * operator) {
    if (strcmp(operator, ARITHMETICA_ASG) == 0) return 0;
    else if (strcmp(operator, ARITHMETICA_LESS) == 0 ||
             strcmp(operator, ARITHMETICA_MORE) == 0 ||
             strcmp(operator, ARITHMETICA_MEQ) ==  0 ||
             strcmp(operator, ARITHMETICA_LEQ) ==  0) return 1;
    else if (strcmp(operator, ARITHMETICA_PLUS) == 0 || strcmp(operator, ARITHMETICA_SUB) == 0) return 2;
    else if (strcmp(operator, ARITHMETICA_MUL) == 0 || strcmp(operator, ARITHMETICA_DIV) == 0) return 3;
    return -1;
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
    else if (strcmp(literal, ARITHMETICA_NOT) == 0) function_pointer = _not;
    else if (strcmp(literal, ARITHMETICA_POW) == 0) function_pointer = _pow;
    else if (strcmp(literal, ARITHMETICA_ASG) == 0) function_pointer = _asg;
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

void constant_destructor(Constant * constant) {
    switch (constant->type_id) {
        case INTEGER: case FLOAT: case STRING:
            free(constant->value);
            break;
        case ARRAY:
            array_destructor(constant->value);
            break;
        case INDEX:
            index_destructor(constant->value);
            break;
    }
    free(constant);
}

void * make_zero_tk() {
    /* Create the zero token for fixing unary plus or minus */
    ExpressionToken * zero_token = malloc(sizeof(ExpressionToken));
    int * zero = malloc(sizeof(int)); *zero = 0;
    char * zero_literal = calloc(sizeof(char), 2); strcpy(zero_literal, "0");
    zero_token->type_id = EXPRESSION_CONSTANT_TK;
    zero_token->literal = zero_literal;
    zero_token->value = zero;
    zero_token->vtype_id = INTEGER;
    return zero_token;
}

Constant * new_constant(char type_id, void * value) {
    Constant * new_constant = malloc(sizeof(Constant));
    new_constant->type_id = type_id; new_constant->value = value;
    return new_constant;
}

void * copy_data(void * src, char type_id) {
    if (type_id == INTEGER) {
        int *tmp = malloc(sizeof(int));
        *tmp = *(int *)src;
        return tmp;
    } else if (type_id == FLOAT) {
        float *tmp = malloc(sizeof(float));
        *tmp = *(float *)src;
        return tmp;
    } else if (type_id == STRING) {
        char *tmp = alloc_string(src);
        return tmp;
    } else if (type_id == ARRAY) {
        Array **tmp = new_array();
        tmp = copy_array(tmp, src);
        return tmp;
    }
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

void * _pow(void * x, void * y){
    return NULL;
}

void * _not(void * x, void * y) {
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

void * _asg(void * x, void * y) {
    /* Assign the literal of y-token with value of x-token into namespace */
    // Y = X, return X
    ExpressionToken * x_tk = x; ExpressionToken * y_tk = y;
    char type_id = x_tk->vtype_id; // type of value
    void * value; value = copy_data(x_tk->value, type_id);

    Constant * copied_value = new_constant(type_id, value); // create the new constant structure with copied value from token
    Node * namespace_object = new_node(faq6(y_tk->literal), copied_value); // create node of namespace binary tree with created constant
    insert_node(namespace, namespace_object); // save it into namespace
    return x_tk;
}

void * _tmp(void * x, void * y) {
    return NULL;
}