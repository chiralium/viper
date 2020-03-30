#include "expression.h"
#include "arithmetica.h"
#include "ViArray.h"

Node * namespace;
char * expression_as_string;

Constant * arithmetica_wrapper(Array ** expression_tokens, Node * current_namespace) {
    expression_as_string = as_string(expression_tokens);
    Constant * result = arithmetica(expression_tokens, current_namespace);
    free(expression_as_string);
    return result;
}

Array ** array_precalc(Array ** array) {
    /* The single element of array is a array of expression tokens */
    int element_counter = 0;
    while (array[element_counter]) {
        if (array[element_counter]->type_id == ARRAY_EL) {
            Array **element = array[element_counter]->element;
            Constant *element_value = arithmetica(element, namespace);
            array[element_counter]->element = element_value->value;
            array[element_counter]->type_id = element_value->type_id;
            free(element_value);
        } else if (array[element_counter]->type_id == ARRAY) array[element_counter]->element = array_precalc(array[element_counter]->element);
        element_counter++;
    }
    return array;
}

Constant * index_precalc(Index * index) {
    Array ** index_parameters = index->params;
    Array ** calculated_index_parameters = new_array();

    /* Calculate the index parameters */
    int params_counter = 0;
    while (params_counter < index->params_count) {
        Constant * calculated_parameter = arithmetica(index_parameters[params_counter]->element, namespace);
        calculated_index_parameters = append(calculated_index_parameters, calculated_parameter->type_id, calculated_parameter->value);
        free(calculated_parameter); free(index_parameters[params_counter]);
        params_counter++;
    }
    free(index_parameters);

    /* Calculate the index object */
    Array ** index_object = index->object;
    Constant * calculated_object = arithmetica(index_object, namespace);
    if (calculated_object->type_id != STRING && calculated_object->type_id != ARRAY) throw_arithmetical_exception(expression_as_string, ARITHMETICA_NOT_ITERABLE_EXCEPTION);
    void * result = _get_by_index(calculated_object, calculated_index_parameters);
    array_destructor(calculated_index_parameters); free(index);
    return result;
}

Constant * arithmetica(Array ** expression_tokens, Node * current_namespace) {
    /* Init global variable of module */
    namespace = current_namespace;
    Array ** postfixed_expression = postfix(expression_tokens);

    /* Convert the all ExpressionToken into Element-structure */
    int counter = 0;
    while(postfixed_expression[counter]) {
        ExpressionToken * token = postfixed_expression[counter]->element;
        Element * elexpr = convert_to_element(token);
        postfixed_expression[counter]->element = elexpr;
        postfixed_expression[counter]->type_id = ELEMENT;
        free(token); counter++;
    }

    Array ** constant_stack = new_array();
    counter = 0;
    while (postfixed_expression[counter]) {
        Element * elexpr = postfixed_expression[counter]->element;
        if (elexpr->type_id == EXPRESSION_CONSTANT_TK) {
            if (elexpr->vtype_id == ARRAY) elexpr->value = array_precalc(elexpr->value);
            else if (elexpr->vtype_id == INDEX) {
                Constant * result = index_precalc(elexpr->value);
                elexpr->vtype_id = result->type_id;
                elexpr->value = result->value;
                free(result);
            }
            constant_stack = append(constant_stack, ELEMENT, elexpr);
        } else {
            Array * stack_el_x = pop_last_el(constant_stack); Array * stack_el_y = pop_last_el(constant_stack);
            if (!stack_el_x || !stack_el_y) throw_arithmetical_exception(expression_as_string, ARITHMETICA_INVALID_EXPRESSION_SYNTAX);

            Element * x_el = stack_el_x->element; free(stack_el_x);
            Element * y_el = stack_el_y->element; free(stack_el_y);
            if (x_el->type_id != EXPRESSION_CONSTANT_TK || y_el->type_id != EXPRESSION_CONSTANT_TK) throw_arithmetical_exception(expression_as_string, ARITHMETICA_INVALID_OPERAND);

            void * (*function_pointer)(void *, void *) = elexpr->value;
            /* if function_pointer is a _tmp function, it is mean, some elexprs like [], cannot be removed at below step of parsing */
            if (function_pointer == _tmp) throw_arithmetical_exception(expression_as_string, ARITHMETICA_UNDEFINED_OPERATOR);
            Element * result_el = function_pointer(x_el, y_el);
            constant_stack = append(constant_stack, ELEMENT, result_el);
        }
        counter++;
    }

    Array * last = pop_last_el(constant_stack);
    if (!is_empty(constant_stack)) throw_arithmetical_exception(expression_as_string, ARITHMETICA_SYNTAX_EXCEPTION);
    Element * result_el = last->element; get_from_namespace(result_el);

    Constant * value = malloc(sizeof(Constant));
    value->type_id = result_el->vtype_id;
    value->value = copy_data(result_el->value, result_el->vtype_id); // copy data from token, cause all token-list must be destroyed

    free(last);
    free(constant_stack);
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
        } else throw_arithmetical_exception(expression_as_string, ARITHMETICA_SYNTAX_EXCEPTION);
    }
    Array * stack_el;
    while (stack_el = pop_last_el(tokens_stack)) {
        ExpressionToken * token = stack_el->element;
        if (token->type_id == OP_OPEN_CBRACK || token->type_id == OP_CLOSE_CBRACK) throw_arithmetical_exception(expression, ARITHMETICA_BRACES_NOT_BALANCED);
        output = append(output, ELEMENT, token); free(stack_el);
    }

    free(expression); free(tokens_stack); free(expression_tokens);

    return output;
}

Element * convert_to_element(void * token) {
    ExpressionToken * tk = token;
    Element * el = malloc(sizeof(Element));

    el->literal = tk->literal;
    el->type_id = tk->type_id;
    el->vtype_id = tk->vtype_id;
    el->value = tk->value;
    /* --- extra fields --- */
    el->origin = 0;
    el->parent_id = 0;
    return el;
}

Array ** fixing_unary_operators(Array ** expression_tokens) {
    int token_counter = 0;
    while (expression_tokens[token_counter]) {
        Element * token = expression_tokens[token_counter]->element;
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
    else if (strcmp(operator, ARITHMETICA_POW) == 0) return 4;
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

Index * new_index(void * object, Array ** params) {
    Index * index = malloc(sizeof(Index));
    index->object = object;
    index->params = params;
    index->params_count = _get_len(params);
    return index;
}

void index_destructor(Index * index) {
    array_destructor(index->object);
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
        case ARRAY: case ARRAY_EL:
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
    } else if (type_id == ARRAY || type_id == ARRAY_EL) {
        void * tmp;
        tmp = copy_array(tmp, src);
        return tmp;
    }
}

int get_int_value(void * elexpr) {
    Element * el = elexpr;
    if (el->vtype_id == INTEGER) return *(int *)(el->value);
    else if (el->vtype_id == FLOAT) return (int)(*(float *)(el->value));
    else {
        char message[EXPRESSION_MAX_LEN]; sprintf(message, ARITHMETICA_TYPECASTING_ERROR, el->vtype_id, 'i');
        throw_typecasting_exception(expression_as_string, message);
    }
}

float get_float_value(void * elexpr) {
    Element * el = elexpr;
    if (el->vtype_id == INTEGER) return (float)(*(int *)(el->value));
    else if (el->vtype_id == FLOAT) return *(float *)(el->value);
    else {
        char message[EXPRESSION_MAX_LEN]; sprintf(message, ARITHMETICA_TYPECASTING_ERROR, el->vtype_id, 'f');
        throw_typecasting_exception(expression_as_string, message);
    }
}

int get_from_namespace(void * elexpr) {
    Element * el = elexpr;
    if (el->vtype_id != UNDEFINED) return 0;
    Constant * value = find_node(namespace, faq6(el->literal));
    if (value == NULL) throw_arithmetical_exception(expression_as_string, ARITHMETICA_UNDEFINED_NAME);
    el->value = copy_data(value->value, value->type_id); el->vtype_id = value->type_id;
}

void * _add(void * x, void * y) {
    // y + x
    Element * x_el = x; Element * y_el = y;
    get_from_namespace(x_el); get_from_namespace(y_el);
    if (y_el->vtype_id == FLOAT || x_el->vtype_id == FLOAT) {
        float * result = malloc(sizeof(float)); *result = get_float_value(y_el) + get_float_value(x_el);
        free(x_el->value);
        x_el->value = result; x_el->vtype_id = FLOAT;
    } else if (y_el->vtype_id == INTEGER) {
        int * result = malloc(sizeof(int)); *result = get_int_value(y_el) + get_int_value(x_el);
        free(x_el->value);
        x_el->value = result; x_el->vtype_id = INTEGER;
    } else if (y_el->vtype_id == STRING) {
        if (x_el->vtype_id != STRING) throw_typecasting_exception(expression_as_string,  ARITHMETICA_STRING_CONCATE_EXCEPTION);
        char stack_tmp_result[ARITHMETICA_MAX_STRING_LEN + 1];
        strcpy(stack_tmp_result, (char *)y_el->value);
        strcat(stack_tmp_result, (char *)x_el->value);
        char * result = alloc_string(stack_tmp_result);
        free(x_el->value);
        x_el->value = result; x_el->vtype_id = STRING;
    } else throw_typecasting_exception(expression_as_string, ARITHMETICA_INVALID_OPERAND);
    return x_el;
}

void * _sub(void * x, void * y) {
    // y - x
    Element * x_el = x; Element * y_el = y;
    get_from_namespace(x_el); get_from_namespace(y_el);
    if(y_el->vtype_id == FLOAT || x_el->vtype_id == FLOAT) {
        float *result = malloc(sizeof(float));
        *result = get_float_value(y_el) - get_float_value(x_el);
        free(x_el->value);
        x_el->value = result;
        x_el->vtype_id = FLOAT;
    } else if (y_el->vtype_id == INTEGER) {
        int * result = malloc(sizeof(int)); *result = get_int_value(y_el) - get_int_value(x_el);
        free(x_el->value);
        x_el->value = result; x_el->vtype_id = INTEGER;
    } else throw_typecasting_exception(expression_as_string, ARITHMETICA_INVALID_OPERAND);
    return x_el;
}

void * _mul(void * x, void * y) {
    // y * x
    Element * x_el = x; Element * y_el = y;
    get_from_namespace(x_el); get_from_namespace(y_el);
    if(y_el->vtype_id == FLOAT || x_el->vtype_id == FLOAT) {
        float *result = malloc(sizeof(float));
        *result = get_float_value(y_el) * get_float_value(x_el);
        free(x_el->value);
        x_el->value = result;
        x_el->vtype_id = FLOAT;
    } else if (y_el->vtype_id == INTEGER) {
        int * result = malloc(sizeof(int)); *result = get_int_value(y_el) * get_int_value(x_el);
        free(x_el->value);
        x_el->value = result; x_el->vtype_id = INTEGER;
    } else throw_typecasting_exception(expression_as_string, ARITHMETICA_INVALID_OPERAND);
    return x_el;
}

void * _div(void * x, void * y) {
    // y / x
    Element * x_el = x; Element * y_el = y;
    get_from_namespace(x_el); get_from_namespace(y_el);
    if(y_el->vtype_id == FLOAT || x_el->vtype_id == FLOAT) {
        float *result = malloc(sizeof(float));
        *result = get_float_value(y_el) / get_float_value(x_el);
        free(x_el->value);
        x_el->value = result;
        x_el->vtype_id = FLOAT;
    } else if (y_el->vtype_id == INTEGER) {
        int * result = malloc(sizeof(int)); *result = get_int_value(y_el) / get_int_value(x_el);
        free(x_el->value);
        x_el->value = result; x_el->vtype_id = INTEGER;
    } else throw_typecasting_exception(expression_as_string, ARITHMETICA_INVALID_OPERAND);
    return x_el;
}

void * _pow(void * x, void * y){
    // y ^ x
    Element * x_el = x; Element * y_el = y;
    get_from_namespace(x_el); get_from_namespace(y_el);
    if(y_el->vtype_id == FLOAT || x_el->vtype_id == FLOAT) {
        float *result = malloc(sizeof(float));
        *result = pow(get_float_value(y_el), get_float_value(x_el));
        free(x_el->value);
        x_el->value = result;
        x_el->vtype_id = FLOAT;
    } else if (y_el->vtype_id == INTEGER) {
        int * result = malloc(sizeof(int)); *result = pow(get_int_value(y_el), get_int_value(x_el));
        free(x_el->value);
        x_el->value = result; x_el->vtype_id = INTEGER;
    } else throw_typecasting_exception(expression_as_string, ARITHMETICA_INVALID_OPERAND);
    return x_el;
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
    Element * x_el = x; Element * y_el = y;
    char type_id = x_el->vtype_id; // type of value
    void * value; value = copy_data(x_el->value, type_id);

    Constant * copied_value = new_constant(type_id, value); // create the new constant structure with copied value from token
    Node * namespace_object = new_node(faq6(y_el->literal), copied_value); // create node of namespace binary tree with created constant
    insert_node(namespace, namespace_object); // save it into namespace
    return x_el;
}

void * _tmp(void * x, void * y) {
    return NULL;
}