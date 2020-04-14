#include "expression.h"

static int _next;
char * expression_as_string;

Array ** expression_lexer(Array ** tokens) {
    Array ** expression = new_array(); // expression is a container that store a ExpressionTokens
    int tokens_counter = 0;
    while (tokens[tokens_counter]) {
        if (tokens[tokens_counter]->type_id == TOKEN) {
            Token * token = (tokens[tokens_counter]->element);
            char * token_literal = token->value;
            Array ** expression_tokens = extract_exp_token(token_literal);
            expression_as_string = as_string(expression_tokens);
            //token_typecasting(expression_tokens);
            expression = append(expression, ARRAY, expression_tokens);
            free(tokens[tokens_counter]); free(expression_as_string);
        } else {
            expression = append(expression, tokens[tokens_counter]->type_id, tokens[tokens_counter]->element);
            free(tokens[tokens_counter]);
        }
        tokens_counter++;
    }
    free(tokens);
    return expression;
}

Array ** extract_exp_token(char * literal) {
    Array ** expression_token = new_array();
    while (*literal) {
        if (*literal == OP_SPACE) pop_first(literal);
        else if (*literal == OP_QUOTE) {
            // this if-statement means the first symbol is quote marks, so next symbol must be aggregate into single tokens
            char * string_literal = cut_string(literal);
            ExpressionToken * string_token = malloc(sizeof(ExpressionToken));
            string_token->type_id = EXPRESSION_CONSTANT_TK;
            string_token->literal = alloc_string(string_literal);
            string_token->vtype_id = STRING;
            string_token->value = string_literal;
            expression_token = append(expression_token, EXP_TK, string_token);
        } else if (!is_in(*literal, EXPRESSION_TERMINATE_OPERATORS) && !is_in(*literal, EXPRESSION_TERMINATE_BRACKETS)) {
            // this if-statement means the first symbol of literal is not an operator and is not a any kind of bracket
            char * constant_literal = cut_constant(literal);
            ExpressionToken * constant_token = malloc(sizeof(ExpressionToken));
            constant_token->type_id = (*literal != OP_OPEN_CBRACK) ? EXPRESSION_CONSTANT_TK : EXPRESSION_CONSTANT_FUNC_TK; // if the next symbol is a '(' and current token is a constant, then this token is function call
            constant_token->literal = constant_literal;
            constant_token->vtype_id = UNDEFINED;
            constant_token->value = NULL;
            expression_token = append(expression_token, EXP_TK, constant_token);
        } else if (is_in(*literal, EXPRESSION_TERMINATE_OPERATORS)) {
            char * operator_literal = cut_operator(literal);
            ExpressionToken * operator_token = malloc(sizeof(ExpressionToken));
            operator_token->type_id = get_token_type(*operator_literal);
            operator_token->literal = operator_literal;
            operator_token->vtype_id = UNDEFINED;
            operator_token->value = NULL;
            expression_token = append(expression_token, EXP_TK, operator_token);
        } else if (is_in(*literal, EXPRESSION_TERMINATE_BRACKETS)) {
            char stack_tmp[2]; stack_tmp[0] = pop_first(literal); stack_tmp[1] = '\0';
            char * bracket_literal = alloc_string(stack_tmp);
            ExpressionToken *bracket_token = malloc(sizeof(ExpressionToken));
            bracket_token->type_id = get_token_type(stack_tmp[0]);
            bracket_token->literal = bracket_literal;
            bracket_token->vtype_id = SYS;
            bracket_token->value = NULL;
            expression_token = append(expression_token, EXP_TK, bracket_token);
        }
    }
    return expression_token;
}

char * cut_constant(char * token) {
    char stack_tmp[EXPRESSION_MAX_LEN + 1]; int tmp_counter = 0;
    char symbol;

    while (*token && !(is_in(*token, EXPRESSION_TERMINATE_OPERATORS) || is_in(*token, EXPRESSION_TERMINATE_BRACKETS))) {
        symbol = pop_first(token);
        if (symbol != FPARSER_EOL) stack_tmp[tmp_counter++] = symbol;
    }

    stack_tmp[tmp_counter] = '\0';
    char * constant_literal = alloc_string(stack_tmp); constant_literal = trim(constant_literal);
    return constant_literal;
}

char * cut_operator(char * token) {
    char stack_tmp[EXPRESSION_MAX_LEN + 1]; int tmp_counter = 0;
    char symbol;

    while (*token && is_in(*token, EXPRESSION_TERMINATE_OPERATORS) && !is_in(*token, EXPRESSION_TERMINATE_BRACKETS)) {
        symbol = pop_first(token);
        stack_tmp[tmp_counter++] = symbol;
    }

    stack_tmp[tmp_counter] = '\0';
    char * operator_literal = alloc_string(stack_tmp); operator_literal = trim(operator_literal);
    return operator_literal;
}

char * cut_string(char * token) {
    char stack_tmp[EXPRESSION_MAX_LEN + 1]; int tmp_counter = 0;
    char symbol; pop_first(token); // pup up the first quote mark

    while (*token && *token != OP_QUOTE) {
        symbol = pop_first(token);
        if (symbol == OP_ESCAPE) {
            symbol = escape2real(pop_first(token));
            if (!symbol) throw_arithmetical_exception(token, EXPRESSION_INVALID_ESCAPE_CHAR);
        }
        stack_tmp[tmp_counter++] = symbol;
    }

    pop_first(token); // pop up the last quote mark
    stack_tmp[tmp_counter] = '\0';
    char * string_literal = alloc_string(stack_tmp);
    return string_literal;
}

Array ** cut_array_element(Array ** exp_tokens) {
    Array ** array_element = new_array(); ExpressionToken * token; int o = 0; int c = 0;
    while (token = get_curr_exp_token(exp_tokens)) {
        (token->type_id == OP_OPEN_BBRACK) ? o++ : (token->type_id == OP_CLOSE_BBRACK) ? c++ : 0;
        if (token->type_id == OP_COMA && o == c) {
            exp_token_destructor(pop_next_exp_token(exp_tokens));
            break;
        } else {
            token = pop_next_exp_token(exp_tokens);
            array_element = append(array_element, EXP_TK, token);
        }
    }
    if (o != c) throw_arithmetical_exception(expression_as_string, EXPRESSION_INVALID_ARRAY_DECLARATION);
    return array_element;
}

Array ** cut_array_body(Array ** exp_tokens, int position) {
    Array ** array = new_array(); ExpressionToken * token;
    int o = 1; int c = 0;
    _next = position + 1;
    while (token = get_curr_exp_token(exp_tokens)) {
        (token->type_id == OP_OPEN_BBRACK) ? o++ : (token->type_id == OP_CLOSE_BBRACK) ? c++ : 0;
        if (token->type_id == OP_CLOSE_BBRACK && o == c) {
            exp_token_destructor(pop_next_exp_token(exp_tokens));
            break;
        } else {
            token = pop_next_exp_token(exp_tokens);
            array = append(array, EXP_TK, token);
        }
    }
    if (o != c) throw_arithmetical_exception(expression_as_string, EXPRESSION_INVALID_ARRAY_DECLARATION);
    _next = 0;
    return array;
}

Array ** array_typecasting(Array ** exp_tokens, int position) {
    Array ** array = new_array();
    Array ** array_body = cut_array_body(exp_tokens, position);

    ExpressionToken * token;
    while (token = get_curr_exp_token(array_body)) {
        if (token->type_id == OP_CLOSE_BBRACK) {
            exp_token_destructor(pop_next_exp_token(array_body));
            break;
        } else if (token->type_id == OP_COMA) throw_arithmetical_exception(expression_as_string, EXPRESSION_INVALID_ARRAY_DECLARATION);
        else {
            Array ** array_element = cut_array_element(array_body);
            token_typecasting(array_element);
            array = append(array, ARRAY_EL, array_element);
        }
    }
    free(array_body);
    return array;
}

Array ** cut_index_parameter(Array ** exp_tokens) {
    Array ** index_parameter = new_array(); ExpressionToken * token; int o = 0; int c = 0;
    while (token = get_curr_exp_token(exp_tokens)) {
        (token->type_id == OP_OPEN_SBRACK) ? o++ : (token->type_id == OP_CLOSE_SBRACK) ? c++ : 0;
        if (token->type_id == OP_COMA && o == c) {
            exp_token_destructor(pop_next_exp_token(exp_tokens));
            break;
        } else {
            token = pop_next_exp_token(exp_tokens);
            index_parameter = append(index_parameter, EXP_TK, token);
        }
    }
    if (o != c) throw_arithmetical_exception(expression_as_string, EXPRESSION_INVALID_INDEX_DECLARATION);
    _next = 0;
    return index_parameter;
}

Array ** cut_index_body(Array ** exp_tokens, int position) {
    ExpressionToken * token = exp_tokens[position]->element;
    if (token->type_id == OP_CLOSE_SBRACK) {
        exp_token_destructor(pop_next_exp_token(exp_tokens));
        return NULL;
    }

    Array ** index_body = new_array();
    int o = 1; int c = 0;
    _next = position;
    while (token = get_curr_exp_token(exp_tokens)) {
        (token->type_id == OP_OPEN_SBRACK) ? o++ : (token->type_id == OP_CLOSE_SBRACK) ? c++ : 0;
        if (token->type_id == OP_CLOSE_SBRACK && o == c) {
            exp_token_destructor(pop_next_exp_token(exp_tokens));
            break;
        } else {
            token = pop_next_exp_token(exp_tokens);
            index_body = append(index_body, EXP_TK, token);
        }
    }
    if (o != c) throw_arithmetical_exception(expression_as_string, EXPRESSION_INVALID_INDEX_DECLARATION);
    _next = 0;
    return index_body;
}

Array ** index_typecasting(Array ** exp_tokens, int position) {
    Array ** index_body = cut_index_body(exp_tokens, position);
    if (index_body == NULL) return NULL;

    Array ** index_parameters = new_array();
    ExpressionToken * token;
    while (token = get_curr_exp_token(index_body)) {
        if (token->type_id == OP_CLOSE_SBRACK) {
            exp_token_destructor(pop_next_exp_token(index_body));
            break;
        } else if (token->type_id == OP_COMA) throw_arithmetical_exception(expression_as_string, EXPRESSION_INVALID_INDEX_DECLARATION);
        else {
            Array ** parameter = cut_index_parameter(index_body);
            token_typecasting(parameter);
            index_parameters = append(index_parameters, ARRAY, parameter);
        }
    }
    if (_get_len(index_parameters) > ARITMHETICA_MAX_INDEX_PARAM) throw_arithmetical_exception(expression_as_string, EXPRESSION_TOO_MUCH_INDEX_PARAMS);
    free(index_body);
    return index_parameters;
}

Array ** cut_index_object(Array ** exp_tokens, int * position) {
    int pos = *position;
    Array ** index_object = new_array(); ExpressionToken * token = exp_tokens[pos]->element;
    int c = 0; int o = 0; int is_bracket_start = token->type_id == OP_CLOSE_CBRACK;
    while (exp_tokens[pos]) {
        token = exp_tokens[pos]->element;
        (token->type_id == OP_OPEN_CBRACK) ? o++ : (token->type_id == OP_CLOSE_CBRACK) ? c++ : 0;
        if (token->type_id == EXPRESSION_OPERATOR_TK && !is_bracket_start && o == c) break;
        else if (!is_bracket_start && c + o != 0) break;
        else {
            token = pop_exp_token(exp_tokens, pos);
            index_object = insert(index_object, EXP_TK, token, 0);
        }
        pos--;
        if (pos < 0) break;
    }
    if (is_empty(index_object)) throw_arithmetical_exception(expression_as_string, EXPRESSION_INVALID_INDEX_DECLARATION);
    *position = pos >= 0 ? pos + 1 : 0;
    return index_object;
}

Array ** cut_arglist_body(Array ** exp_tokens, int position) {
    Array ** arglist_body = new_array(); ExpressionToken * token;
    int o = 1; int c = 0;
    _next = position;
    exp_token_destructor(pop_next_exp_token(exp_tokens));
    while (token = get_curr_exp_token(exp_tokens)) {
        (token->type_id == OP_OPEN_CBRACK) ? o++ : (token->type_id == OP_CLOSE_CBRACK) ? c++ : 0;
        if (token->type_id == OP_CLOSE_CBRACK && o == c) {
            exp_token_destructor(pop_next_exp_token(exp_tokens));
            break;
        } else {
            token = pop_next_exp_token(exp_tokens);
            arglist_body = append(arglist_body, EXP_TK, token);
        }
    }
    if (o != c) throw_arithmetical_exception(expression_as_string, EXPRESSION_INVALID_FUNCTION_CALL);
    _next = 0;
    return arglist_body;
}

Array ** cut_argument(Array ** exp_tokens) {
    Array ** argument = new_array(); ExpressionToken * token; int o = 0; int c = 0;
    while (token = get_curr_exp_token(exp_tokens)) {
        (token->type_id == OP_OPEN_CBRACK) ? o++ : (token->type_id == OP_CLOSE_CBRACK) ? c++ : 0;
        if (token->type_id == OP_COMA && o == c) {
            exp_token_destructor(pop_next_exp_token(exp_tokens));
            break;
        } else {
            token = pop_next_exp_token(exp_tokens);
            argument = append(argument, EXP_TK, token);
        }
    }
    if (o != c) throw_arithmetical_exception(expression_as_string, EXPRESSION_INVALID_FUNCTION_CALL);
    return argument;
}

Array ** cut_function_arglist(Array ** exp_tokens, int position) {
    Array ** arglist = new_array();
    Array ** arglist_body = cut_arglist_body(exp_tokens, position);

    ExpressionToken * token;
    while (token = get_curr_exp_token(arglist_body)) {
        if (token->type_id == OP_CLOSE_CBRACK) {
            exp_token_destructor(pop_next_exp_token(arglist_body));
            break;
        } else if (token->type_id == OP_COMA) throw_arithmetical_exception(expression_as_string, EXPRESSION_INVALID_FUNCTION_ARGLIST);
        else {
            Array ** argument = cut_argument(arglist_body);
            token_typecasting(argument);
            arglist = append(arglist, ARRAY, argument);
        }
    }
    free(arglist_body);
    return arglist;
}

void token_typecasting(Array ** exp_tokens) {
    int counter = 0;
    while (exp_tokens[counter]) {
        ExpressionToken * token = exp_tokens[counter]->element;
        if (token->type_id == OP_OPEN_BBRACK) {
            /* {...} */
            Array **  array = array_typecasting(exp_tokens, counter);
            ExpressionToken *array_tk = malloc(sizeof(ExpressionToken));
            array_tk->type_id = EXPRESSION_CONSTANT_TK;
            array_tk->vtype_id = ARRAY;
            array_tk->value = array;
            array_tk->literal = alloc_string("<array>");
            exp_token_destructor(exp_tokens[counter]->element);
            exp_tokens[counter]->element = array_tk;
        } else if (token->type_id == OP_OPEN_SBRACK) {
            /* <EXP>[...] */
            if (--counter < 0) throw_arithmetical_exception(expression_as_string, EXPRESSION_INVALID_INDEX_DECLARATION);
            Array **index_object = cut_index_object(exp_tokens,
                                                    &counter); // this function will decrease the counter value for each object's tokens
            Array **index_parameters = index_typecasting(exp_tokens, counter + 1);
            Index *index = new_index(index_object, index_parameters);
            ExpressionToken *index_tk = malloc(sizeof(ExpressionToken));
            index_tk->type_id = EXPRESSION_CONSTANT_TK;
            index_tk->vtype_id = INDEX;
            index_tk->value = index;
            index_tk->literal = alloc_string(expression_as_string);
            exp_token_destructor(exp_tokens[counter]->element);
            exp_tokens[counter]->element = index_tk;
        } else if (token->type_id == EXPRESSION_CONSTANT_FUNC_TK) {
            /* foo(x1, x2, ... , xN) */
            char * function_name = token->literal;
            Array ** function_arglist = cut_function_arglist(exp_tokens, counter + 1);
            FuncCall * function_call = new_func_call(function_name, function_arglist);
            ExpressionToken * function_tk = malloc(sizeof(ExpressionToken));
            function_tk->type_id = EXPRESSION_CONSTANT_FUNC_TK;
            function_tk->vtype_id = FUNCTION_RES;
            function_tk->value = function_call;
            function_tk->literal = alloc_string(function_name);
            exp_token_destructor(exp_tokens[counter]->element);
            exp_tokens[counter]->element = function_tk;
        } else allocate_token_value(token);
        counter++;
    }
}


int allocate_token_value(ExpressionToken * exp_token) {
    char * literal = exp_token->literal; void * (*function_pointer)(void *, void *) = NULL;
    if (exp_token->value != NULL || exp_token->vtype_id == SYS) return 0;
    if (is_int_number(literal)) {
        int * value = malloc(sizeof(int));
        *value = atoi(literal);
        exp_token->value = value;
        exp_token->vtype_id = INTEGER;
    } else if (is_float_number(literal)) {
        float *value = malloc(sizeof(float));
        *value = atof(literal);
        exp_token->value = value;
        exp_token->vtype_id = FLOAT;
    } else if (function_pointer = assign_function(literal)) {
        exp_token->type_id = EXPRESSION_OPERATOR_TK;
        exp_token->value = function_pointer;
        exp_token->vtype_id = get_operator_type(literal);
    } else if (is_name(literal)) {
        exp_token->value = NULL;
        exp_token->vtype_id = UNDEFINED;
    } else throw_arithmetical_exception(literal, EXPRESSION_UNDEFINED_TOKEN);
}

int is_in(char symbol, char * stop_symbols) {
    int counter = 0;
    while (stop_symbols[counter] != symbol && stop_symbols[counter]) counter++;
    return !stop_symbols[counter] ? 0 : 1;
}

int is_int_number(char * literal) {
    int is_number = 0;
    while (*literal) {
        if (!(is_number = isdigit(*literal))) break;
        literal++;
    }
    return is_number;
}

int is_float_number(char * literal) {
    int is_number = 0;
    while (*literal) {
        if (!(is_number = (isdigit(*literal) || *literal == '.'))) break;
        literal++;
    }
    return is_number;
}

int is_name(char * literal) {
    int is_name = 0;
    if (isdigit(*literal)) return 0;
    while (*literal) {
        if (!(is_name = isdigit(*literal) || isalpha(*literal))) break;
        literal++;
    }
    return is_name;
}

ExpressionToken * get_next_exp_token(Array ** exp_tokens) {
    if (exp_tokens[_next]) return (ExpressionToken *)(exp_tokens[_next++]->element);
    else return 0;
}

ExpressionToken * get_curr_exp_token(Array ** exp_tokens) {
    if (exp_tokens[_next]) return  (ExpressionToken *)(exp_tokens[_next]->element);
    return 0;
}

ExpressionToken * pop_next_exp_token(Array ** exp_tokens) {
    if (exp_tokens[_next]) {
        int length = _get_len(exp_tokens);
        ExpressionToken * next = exp_tokens[_next]->element; free(exp_tokens[_next]); // free the element of Array **
        memcpy(exp_tokens + _next, exp_tokens + (_next + 1), sizeof(Array *) * (length - _next)); // and write the next element at this position
        return next;
    } else {
        return 0;
    }
}

ExpressionToken * pop_exp_token(Array ** exp_tokens, int position) {
    if (exp_tokens[position]) {
        int length = _get_len(exp_tokens);
        ExpressionToken * token = exp_tokens[position]->element; free(exp_tokens[position]);
        memcpy(exp_tokens + position, exp_tokens + (position + 1), sizeof(Array *) * (length - position));
        return token;
    } else return 0;
}

void exp_token_destructor(ExpressionToken * token) {
    switch (token->vtype_id) {
    case INTEGER: case FLOAT: case STRING: case UNDEFINED:
            free(token->value);
            break;
        case ARRAY: case ARRAY_EL:
            array_destructor(token->value);
            break;
        case INDEX:
            index_destructor(token->value);
            break;
        case FUNCTION_RES:
            func_call_destructor(token->value);
            break;
    }

    if (token->literal != NULL) free(token->literal);
    free(token);
}

void exp_tokens_destructor(Array ** tokens) {
    int counter = 0;
    while (tokens[counter]) {
        if (tokens[counter]->type_id == ARRAY) array_destructor(tokens[counter]->element);
        free(tokens[counter]);
        counter++;
    }
    free(tokens);
}

char get_token_type(char symbol) {
    switch (symbol) {
        case OP_OPEN_BBRACK: return OP_OPEN_BBRACK;
        case OP_CLOSE_BBRACK: return OP_CLOSE_BBRACK;
        case OP_OPEN_SBRACK: return OP_OPEN_SBRACK;
        case OP_CLOSE_SBRACK: return OP_CLOSE_SBRACK;
        case OP_OPEN_CBRACK: return OP_OPEN_CBRACK;
        case OP_CLOSE_CBRACK: return OP_CLOSE_CBRACK;
        case OP_QUOTE: return OP_QUOTE;

        case OP_PLUS: return OP_PLUS;
        case OP_MINUS: return OP_MINUS;
        case OP_MUL: return OP_MUL;
        case OP_DIV: return OP_DIV;

        case OP_COMA: return OP_COMA;
        default: return EXPRESSION_OPERATOR_TK;
    }
}

char get_operator_type(char * operator) {
    if (strcmp(operator, ARITHMETICA_PLUS) == 0) return OPERATOR_PLUS;
    else if (strcmp(operator, ARITHMETICA_SUB) == 0) return OPERATOR_MINUS;
    else if (strcmp(operator, ARITHMETICA_MUL) == 0) return OPERATOR_MUL;
    else if (strcmp(operator, ARITHMETICA_DIV) == 0) return OPERATOR_DIV;
    else if (strcmp(operator, ARITHMETICA_POW) == 0) return OPERATOR_POW;
    else if (strcmp(operator, ARITHMETICA_NOT) == 0) return OPERATOR_NOT;
    else if (strcmp(operator, ARITHMETICA_MORE) == 0) return OPERATOR_MORE;
    else if (strcmp(operator, ARITHMETICA_LESS) == 0) return OPERATOR_LESS;
    else if (strcmp(operator, ARITHMETICA_MEQ) == 0) return OPERATOR_MEQ;
    else if (strcmp(operator, ARITHMETICA_LEQ) == 0) return OPERATOR_LEQ;
    else if (strcmp(operator, ARITHMETICA_EQ) == 0) return OPERATOR_EQ;
    else if (strcmp(operator, ARITHMETICA_ASG) == 0) return OPERATOR_ASG;
    else if (strcmp(operator, ARITHMETICA_ASC) == 0) return OPERATOR_ASC;
    return 0;
}

char escape2real(char symbol) {
    switch(symbol) {
        case 'n': return '\n';
        case 't': return '\t';
        case '\\': return '\\';
        case '\"': return '"';
        default: return 0;
    }
}

char * as_string(Array ** exp_tokens) {
    char stack_tmp[EXPRESSION_MAX_LEN + 1] = "\0"; int token_counter = 0;
    ExpressionToken * token;
    while (exp_tokens[token_counter]) {
        token = exp_tokens[token_counter++]->element;
        if (token->literal != NULL) strcat(stack_tmp, token->literal);
    }
    char * string = alloc_string(stack_tmp);
    return string;
}