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
            array[element_counter]->element = element_value;
            array[element_counter]->type_id = CONSTANT;
            //free(element_value);
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
        calculated_index_parameters = append(calculated_index_parameters, CONSTANT, calculated_parameter);
        free(index_parameters[params_counter]);
        params_counter++;
    }
    free(index_parameters);

    /* Calculate the index object */
    Array ** index_object = index->object;
    Constant * calculated_object = arithmetica(index_object, namespace);
    if (calculated_object->type_id != STRING && calculated_object->type_id != VIARRAY) throw_arithmetical_exception(expression_as_string, ARITHMETICA_NOT_ITERABLE_EXCEPTION);
    void * result = get_by_index(calculated_object, calculated_index_parameters);
    array_destructor(calculated_index_parameters);
    free(index);
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
            if (elexpr->vtype_id == ARRAY) {
                Array ** array = array_precalc(elexpr->value);
                Node * viarray = new_viarray(array);
                elexpr->vtype_id = VIARRAY;
                elexpr->value = viarray;
                elexpr->origin = NULL;
            } else if (elexpr->vtype_id == INDEX) {
                Index * index = elexpr->value;
                Constant * result = index_precalc(index);
                elexpr->vtype_id = result->type_id;
                elexpr->value = result->value; //copy_data(result->value, result->type_id);
                elexpr->origin = result->origin;
                free(result);
            }
            constant_stack = append(constant_stack, ELEMENT, elexpr); free(postfixed_expression[counter]);
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

            free(postfixed_expression[counter]);
            free(elexpr->literal); free(elexpr);
            element_destructor(y_el);
        }
        counter++;
    }

    Array * last = pop_last_el(constant_stack);
    if (!is_empty(constant_stack)) throw_arithmetical_exception(expression_as_string, ARITHMETICA_SYNTAX_EXCEPTION);
    Element * result_el = last->element;

    if (get_from_namespace(result_el) == -1) throw_arithmetical_exception(expression_as_string, ARITHMETICA_UNDEFINED_NAME);

    Constant * value = malloc(sizeof(Constant));
    value->type_id = result_el->vtype_id;
    value->value = result_el->value;
    value->origin = result_el->origin;

    free(result_el->literal); free(result_el);

    free(last);
    free(constant_stack);
    free(postfixed_expression);
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
    index->params_count = (params != NULL) ? _get_len(params) : 0; // if the index is NULL
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

void element_destructor(Element * elexpr) {
    if (elexpr->origin == NULL) {
        if (is_simple_data(elexpr->vtype_id)) free(elexpr->value);
    }
    free(elexpr->literal);
    free(elexpr);
}

void constant_destructor(Constant * constant) {
    if (constant->origin == NULL) {
        switch (constant->type_id) {
            case INTEGER:
            case FLOAT:
            case STRING:
                free(constant->value);
                break;
            case VIARRAY:
                namespace_destructor(constant->value);
                break;
            case ARRAY:
            case ARRAY_EL:
                array_destructor(constant->value);
                break;
            case INDEX:
                index_destructor(constant->value);
                break;
        }
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
    new_constant->type_id = type_id;
    new_constant->value = value;
    new_constant->origin = NULL;
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
    } else {
        return src;
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
    Node * node = find_node(namespace, faq6(el->literal));
    if (node == NULL) return -1;
    Constant * value = node->value;

    if (is_simple_data(value->type_id)) {
        el->value = copy_data(value->value, value->type_id);
        el->vtype_id = value->type_id;
        el->origin = NULL;
    } else {
        el->value = value->value;
        el->vtype_id = value->type_id;
        el->origin = node;
    }
}

void * _add(void * x, void * y) {
    // y + x
    Element * x_el = x; Element * y_el = y;
    Element * result_el = NULL;
    if (get_from_namespace(x_el) == -1 || get_from_namespace(y_el) == -1) throw_arithmetical_exception(expression_as_string, ARITHMETICA_UNDEFINED_NAME);
    if (y_el->vtype_id == FLOAT || x_el->vtype_id == FLOAT) {
        float * result = malloc(sizeof(float)); *result = get_float_value(y_el) + get_float_value(x_el);

        result_el = malloc(sizeof(Element));
        result_el->origin = NULL;
        result_el->type_id = x_el->type_id;
        result_el->vtype_id = FLOAT;
        result_el->value = result;
        if (x_el->literal != NULL) result_el->literal = alloc_string(x_el->literal);
        else result_el->literal = NULL;

        element_destructor(x_el);
    } else if (y_el->vtype_id == INTEGER) {
        int * result = malloc(sizeof(int)); *result = get_int_value(y_el) + get_int_value(x_el);

        result_el = malloc(sizeof(Element));
        result_el->origin = NULL;
        result_el->type_id = x_el->type_id;
        result_el->vtype_id = INTEGER;
        result_el->value = result;
        if (x_el->literal != NULL) result_el->literal = alloc_string(x_el->literal);
        else result_el->literal = NULL;

        element_destructor(x_el);
    } else if (y_el->vtype_id == STRING) {
        if (x_el->vtype_id != STRING) throw_typecasting_exception(expression_as_string,  ARITHMETICA_STRING_CONCATE_EXCEPTION);
        char stack_tmp_result[ARITHMETICA_MAX_STRING_LEN + 1];
        strcpy(stack_tmp_result, (char *)y_el->value);
        strcat(stack_tmp_result, (char *)x_el->value);
        char * result = alloc_string(stack_tmp_result);

        result_el = malloc(sizeof(Element));
        result_el->origin = NULL;
        result_el->type_id = x_el->type_id;
        result_el->vtype_id = STRING;
        result_el->value = result;
        if (x_el->literal != NULL) result_el->literal = alloc_string(x_el->literal);
        else result_el->literal = NULL;

        element_destructor(x_el);
    } else throw_typecasting_exception(expression_as_string, ARITHMETICA_INVALID_OPERAND);
    return result_el;
}

void * _sub(void * x, void * y) {
    // y - x
    Element * result_el = NULL;
    Element * x_el = x; Element * y_el = y;
    if (get_from_namespace(x_el) == -1 || get_from_namespace(y_el) == -1) throw_arithmetical_exception(expression_as_string, ARITHMETICA_UNDEFINED_NAME);
    if(y_el->vtype_id == FLOAT || x_el->vtype_id == FLOAT) {
        float *result = malloc(sizeof(float));
        *result = get_float_value(y_el) - get_float_value(x_el);

        result_el = malloc(sizeof(Element));
        result_el->origin = NULL;
        result_el->type_id = x_el->type_id;
        result_el->vtype_id = FLOAT;
        result_el->value = result;
        if (x_el->literal != NULL) result_el->literal = alloc_string(x_el->literal);
        else result_el->literal = NULL;

        element_destructor(x_el);
    } else if (y_el->vtype_id == INTEGER) {
        int * result = malloc(sizeof(int)); *result = get_int_value(y_el) - get_int_value(x_el);

        result_el = malloc(sizeof(Element));
        result_el->origin = NULL;
        result_el->type_id = x_el->type_id;
        result_el->vtype_id = INTEGER;
        result_el->value = result;
        if (x_el->literal != NULL) result_el->literal = alloc_string(x_el->literal);
        else result_el->literal = NULL;

        element_destructor(x_el);
    } else throw_typecasting_exception(expression_as_string, ARITHMETICA_INVALID_OPERAND);
    return result_el;
}

void * _mul(void * x, void * y) {
    // y * x
    Element * result_el = NULL;
    Element * x_el = x; Element * y_el = y;
    if (get_from_namespace(x_el) == -1 || get_from_namespace(y_el) == -1) throw_arithmetical_exception(expression_as_string, ARITHMETICA_UNDEFINED_NAME);
    if(y_el->vtype_id == FLOAT || x_el->vtype_id == FLOAT) {
        float *result = malloc(sizeof(float)); *result = get_float_value(y_el) * get_float_value(x_el);

        result_el = malloc(sizeof(Element));
        result_el->origin = NULL;
        result_el->type_id = x_el->type_id;
        result_el->vtype_id = FLOAT;
        result_el->value = result;
        if (x_el->literal != NULL) result_el->literal = alloc_string(x_el->literal);
        else result_el->literal = NULL;

        element_destructor(x_el);
    } else if (y_el->vtype_id == INTEGER) {
        int * result = malloc(sizeof(int)); *result = get_int_value(y_el) * get_int_value(x_el);
        result_el = malloc(sizeof(Element));
        result_el->origin = NULL;
        result_el->type_id = x_el->type_id;
        result_el->vtype_id = INTEGER;
        result_el->value = result;
        if (x_el->literal != NULL) result_el->literal = alloc_string(x_el->literal);
        else result_el->literal = NULL;

        element_destructor(x_el);
    } else throw_typecasting_exception(expression_as_string, ARITHMETICA_INVALID_OPERAND);
    return result_el;
}

void * _div(void * x, void * y) {
    // y / x
    Element * result_el = NULL;
    Element * x_el = x; Element * y_el = y;
    if (get_from_namespace(x_el) == -1 || get_from_namespace(y_el) == -1) throw_arithmetical_exception(expression_as_string, ARITHMETICA_UNDEFINED_NAME);
    if(y_el->vtype_id == FLOAT || x_el->vtype_id == FLOAT) {
        float *result = malloc(sizeof(float)); *result = get_float_value(y_el) / get_float_value(x_el);
        result_el = malloc(sizeof(Element));
        result_el->origin = NULL;
        result_el->type_id = x_el->type_id;
        result_el->vtype_id = FLOAT;
        result_el->value = result;
        if (x_el->literal != NULL) result_el->literal = alloc_string(x_el->literal);
        else result_el->literal = NULL;

        element_destructor(x_el);
    } else if (y_el->vtype_id == INTEGER) {
        int * result = malloc(sizeof(int)); *result = get_int_value(y_el) / get_int_value(x_el);
        result_el = malloc(sizeof(Element));
        result_el->origin = NULL;
        result_el->type_id = x_el->type_id;
        result_el->vtype_id = INTEGER;
        result_el->value = result;
        if (x_el->literal != NULL) result_el->literal = alloc_string(x_el->literal);
        else result_el->literal = NULL;

        element_destructor(x_el);
    } else throw_typecasting_exception(expression_as_string, ARITHMETICA_INVALID_OPERAND);
    return result_el;
}

void * _pow(void * x, void * y){
    // y ^ x
    Element * result_el = NULL;
    Element * x_el = x; Element * y_el = y;
    if (get_from_namespace(x_el) == -1 || get_from_namespace(y_el) == -1) throw_arithmetical_exception(expression_as_string, ARITHMETICA_UNDEFINED_NAME);
    if(y_el->vtype_id == FLOAT || x_el->vtype_id == FLOAT) {
        float *result = malloc(sizeof(float)); *result = pow(get_float_value(y_el), get_float_value(x_el));
        result_el = malloc(sizeof(Element));
        result_el->origin = NULL;
        result_el->type_id = x_el->type_id;
        result_el->vtype_id = FLOAT;
        result_el->value = result;
        if (x_el->literal != NULL) result_el->literal = alloc_string(x_el->literal);
        else result_el->literal = NULL;

        element_destructor(x_el);
    } else if (y_el->vtype_id == INTEGER) {
        int * result = malloc(sizeof(int)); *result = pow(get_int_value(y_el), get_int_value(x_el));
        result_el = malloc(sizeof(Element));
        result_el->origin = NULL;
        result_el->type_id = x_el->type_id;
        result_el->vtype_id = INTEGER;
        result_el->value = result;
        if (x_el->literal != NULL) result_el->literal = alloc_string(x_el->literal);
        else result_el->literal = NULL;

        element_destructor(x_el);
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
    if (get_from_namespace(x_el) == -1) throw_arithmetical_exception(expression_as_string, ARITHMETICA_UNDEFINED_NAME);

    if (y_el->literal == NULL) throw_arithmetical_exception(expression_as_string, ARITHMETICA_OBJECT_NOT_ASSIGNABLE);
    void * origin = find_node(namespace, faq6(y_el->literal));
    if (!is_name(y_el->literal) && origin == NULL && y_el->origin == NULL) throw_arithmetical_exception(expression_as_string, ARITHMETICA_OBJECT_NOT_ASSIGNABLE);

    if (origin || y_el->origin) {
        (origin) ? y_el->origin = origin : NULL;
        // that means the variable Y is already set
        Node * previuos_value = y_el->origin;
        constant_destructor(previuos_value->value); // destroy the previous value in namespace
        Constant * new_value = new_constant(x_el->vtype_id, x_el->value);
        previuos_value->value = new_value;
        if (x_el->origin != NULL) {
            Node * node = x_el->origin;
            Constant * complex_value =  node->value;
            complex_value->origin = previuos_value;
        }
        x_el->origin = previuos_value;
    } else {
        // that means the variable Y is initialized
        Node * namespace_object; Constant * y_value;
        if (x_el->origin) {
            Node * node = x_el->origin;
            if (is_simple_data(x_el->vtype_id)) {
                // if an simple data and x_el have origin that means the element is a stored in complex structure
                void * copied_value = copy_data(x_el->value, x_el->vtype_id);
                y_value = new_constant(x_el->vtype_id, copied_value);
                namespace_object = new_node(faq6(y_el->literal), y_value);
                x_el->origin = namespace_object;
            } else {
                Constant * complex_value = node->value;
                y_value = new_constant(complex_value->type_id, complex_value->value);
                namespace_object = new_node(faq6(y_el->literal), y_value);
                x_el->origin = namespace_object;
                complex_value->origin = namespace_object;
            }
        } else {
            // if X variable is an simple data like string or numbers
            y_value = new_constant(x_el->vtype_id, x_el->value);
            namespace_object = new_node(faq6(y_el->literal), y_value);
            x_el->origin = namespace_object;
        }
        insert_node(namespace, namespace_object);
    }
    return x_el;
}

void * _tmp(void * x, void * y) {
    return NULL;
}

int is_simple_data(char type_id) {
    switch (type_id) {
        case INTEGER:
        case FLOAT:
        case STRING:
            return 1;
        case VIARRAY:
            return 0;
        default:
            return 0;
    }
}