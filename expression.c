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
            token_typecast(expression_tokens);
            expression = append(expression, ARRAY, expression_tokens);
            free(tokens[tokens_counter]);
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
            string_token->literal = NULL;
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


Array ** cut_array_el(Array ** exp_tokens) {
    Array ** tokens = new_array(); ExpressionToken * token;
    while (token = get_curr_exp_token(exp_tokens)) {
        if (token->type_id != OP_COMA && token->type_id != OP_CLOSE_BBRACK) {
            token = pop_next_exp_token(exp_tokens);
            tokens = append(tokens, EXP_TK, token);
        } else break;
    }
    return tokens;
}

Array ** cut_array(Array ** exp_tokens) {
    Array ** array = new_array(); ExpressionToken * token; int coma_counter = 0;
    while (token = get_curr_exp_token(exp_tokens)) {
        if (token->type_id == OP_CLOSE_BBRACK) {
            exp_token_destructor(pop_next_exp_token(exp_tokens)); // free the }-token
            break;
        } else if (token->type_id == OP_COMA) {
            if (++coma_counter > _get_len(array) || is_empty(array)) throw_arithmetical_exception(expression_as_string, EXPRESSION_INVALID_ARRAY_DECLARATION);
            exp_token_destructor(pop_next_exp_token(exp_tokens));
        } else if (token->type_id == OP_OPEN_BBRACK) {
              exp_token_destructor(pop_next_exp_token(exp_tokens)); // free the {-token
              array = append(array, ARRAY, cut_array(exp_tokens));
        } else if (token->type_id != OP_OPEN_BBRACK) array = append(array, ARRAY_EL, cut_array_el(exp_tokens));
    }
    return array;
}

Array ** cut_index_body(Array ** exp_tokens) {
    Array ** tokens = new_array(); ExpressionToken * token;
    int o = 1; int c = 0;
    while (token = get_curr_exp_token(exp_tokens)) {
        (token->type_id == OP_OPEN_SBRACK) ? o++ : (token->type_id == OP_CLOSE_SBRACK) ? c++ : 0;
        if (token->type_id == OP_CLOSE_SBRACK && o == c){
            exp_token_destructor(pop_next_exp_token(exp_tokens));
            break;
        } else {
            token = pop_next_exp_token(exp_tokens);
            tokens = append(tokens, EXP_TK,token);
        }
    }
    return tokens;
}

Array ** cut_index_el(Array ** exp_tokens) {
    Array ** tokens = new_array(); ExpressionToken * token; int o = 0; int c = 0;
    while (token = get_curr_exp_token(exp_tokens)) {
        (token->type_id == OP_OPEN_SBRACK) ? o++ : (token->type_id == OP_CLOSE_SBRACK) ? c++ : 0;
        if (o != c) {
            token = pop_next_exp_token(exp_tokens);
            tokens = append(tokens, EXP_TK, token);
        } else {
            if (token->type_id != OP_COMA) {
                token = pop_next_exp_token(exp_tokens);
                tokens = append(tokens, EXP_TK, token);
            } else break;
        }
    }
    return tokens;
}

Array ** cut_index(Array ** exp_tokens) {
    Array ** index_params = new_array(); ExpressionToken * token; int coma_counter = 0; int _old_next = _next;
    Array ** index_body = cut_index_body(exp_tokens);
    _next = 0;
    while (token = get_curr_exp_token(index_body)) {
        if (token->type_id == OP_CLOSE_SBRACK) {
            exp_token_destructor(pop_next_exp_token(index_body)); // free the ]-token
            break;
        } else if (token->type_id == OP_COMA) {
            if (++coma_counter > _get_len(index_params) || is_empty(index_params)) throw_arithmetical_exception(expression_as_string, EXPRESSION_INVALID_INDEX_DECLARATION);
            exp_token_destructor(pop_next_exp_token(index_body));
        } else if (token->type_id != OP_OPEN_SBRACK) {
            index_params = append(index_params, ARRAY, cut_index_el(index_body));
            if (_get_len(index_params) > ARITMHETICA_MAX_INDEX_PARAM) throw_arithmetical_exception(expression_as_string, EXPRESSION_TOO_MUCH_INDEX_PARAMS);
        }
    }
    free(index_body);
    _next = _old_next;
    return index_params;
}

Array ** cut_single_arg(Array ** exp_tokens) {
    char * tokens_as_string = as_string(exp_tokens);
    Array ** single_arg_tokens = new_array(); ExpressionToken * token; int o = 1; int c = 0;
    while (token = get_curr_exp_token(exp_tokens)) {
        (token->type_id == OP_OPEN_CBRACK) ? o++ : (token->type_id == OP_CLOSE_CBRACK) ? c++ : 0;
        if (token->type_id == OP_CLOSE_CBRACK && c == o) break;
        if (token->type_id != OP_COMA) {
            token = pop_next_exp_token(exp_tokens);
            single_arg_tokens = append(single_arg_tokens, EXP_TK, token);
        } else break;
    }
    if (o != c) throw_arithmetical_exception(tokens_as_string, EXPRESSION_INVALID_FUNCTION_CALL);
    free(tokens_as_string);
    return single_arg_tokens;
}

Array ** cut_arglist(Array ** exp_tokens) {
    Array ** arg_list = new_array(); ExpressionToken * token; _next++;
    exp_token_destructor(pop_next_exp_token(exp_tokens));
    while (token = get_curr_exp_token(exp_tokens)) {
        if (token->type_id == OP_COMA) exp_token_destructor(pop_next_exp_token(exp_tokens));
        else if (token->type_id != OP_CLOSE_CBRACK) {
            arg_list = append(arg_list, ARRAY, cut_single_arg(exp_tokens));
        } else {
            exp_token_destructor(pop_next_exp_token(exp_tokens));
            break;
        }
    }
    return arg_list;
}

void token_typecast(Array ** exp_tokens) {
    expression_as_string = as_string(exp_tokens);
    typecast_constant(exp_tokens);
    typecast_array(exp_tokens);
    typecast_index(exp_tokens);
    typecast_function(exp_tokens);
    free(expression_as_string);
}

void typecast_constant(Array ** exp_tokens) {
    ExpressionToken * token; int token_counter = 0;
    while (exp_tokens[token_counter]) {
        token = exp_tokens[token_counter++]->element;
        allocate_token_value(token);
    }
}

void typecast_array(Array ** exp_tokens) {
    ExpressionToken * token;
    while (token = get_curr_exp_token(exp_tokens)) {
        if (token->type_id == OP_OPEN_BBRACK) {
            int position = _next; _next++;
            Array **array = cut_array(exp_tokens);
            ExpressionToken *array_tk = malloc(sizeof(ExpressionToken));
            array_tk->type_id = EXPRESSION_CONSTANT_TK;
            array_tk->literal = NULL;
            array_tk->vtype_id = ARRAY;
            array_tk->value = array;
            exp_token_destructor(exp_tokens[position]->element); // free the current {-token =
            exp_tokens[position]->element = array_tk;
        } else _next++;
    }
    _next = 0;
}

void typecast_index(Array ** exp_tokens) {
    ExpressionToken * token;
    while (token = get_curr_exp_token(exp_tokens)) {
        if (token->type_id == OP_OPEN_SBRACK) {
            int position = _next - 1; exp_token_destructor(pop_next_exp_token(exp_tokens));
            if (position < 0) throw_arithmetical_exception(expression_as_string, EXPRESSION_INVALID_INDEX_DECLARATION);
            Array ** index_params = cut_index(exp_tokens);
            Index * index = new_index(exp_tokens[position]->element, index_params, _get_len(index_params));
            ExpressionToken * index_tk = malloc(sizeof(ExpressionToken));
            index_tk->type_id = EXPRESSION_CONSTANT_TK;
            index_tk->literal = NULL;
            index_tk->vtype_id = INDEX;
            index_tk->value = index;
            exp_tokens[position]->element = index_tk;
        } else _next++;
    }
    _next = 0;
}

void typecast_function(Array ** exp_tokens) {
    ExpressionToken * token;
    while (token = get_curr_exp_token(exp_tokens)) {
        if (token->type_id == EXPRESSION_CONSTANT_FUNC_TK) {
            int position = _next;
            Array ** arg_list = cut_arglist(exp_tokens);
            FuncCall * funccall = new_func_call(token->literal, arg_list);
            ExpressionToken * funccall_tk = malloc(sizeof(ExpressionToken));
            funccall_tk->type_id = EXPRESSION_CONSTANT_TK;
            funccall_tk->literal = NULL;
            funccall_tk->vtype_id = FUNCTION_RES;
            funccall_tk->value = funccall;
            exp_token_destructor(exp_tokens[position]->element);
            exp_tokens[position]->element = funccall_tk;
        } else _next++;
    }
    _next = 0;
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
    return 0;
}

char escape2real(char symbol) {
    switch(symbol) {
        case 'n': return '\n';
        case 't': return '\t';
        case '\\': return '\\';
        case '\"': return '"';
        default: return NULL;
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