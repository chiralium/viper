#include "expression.h"

static int _next;

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
        } else expression = append(expression, tokens[tokens_counter]->type_id, tokens[tokens_counter]->element);
        tokens_counter++;
    }
    return expression;
}

Array ** extract_exp_token(char * literal) {
    Array ** expression_token = new_array();
    while (*literal) {
        if (*literal == FPARSER_SPACE) pop_first(literal);
        else if (!is_in(*literal, EXPRESSION_TERMINATE_OPERATORS) && !is_in(*literal, EXPRESSION_TERMINATE_BRACKETS)) {
            // this if-statement means the first symbol of literal is not an operator and is not a any kind of bracket
            char *constant_literal = cut_constant(literal);
            ExpressionToken *constant_token = malloc(sizeof(ExpressionToken));
            constant_token->type_id = EXPRESSION_CONSTANT_TK;
            constant_token->literal = constant_literal;
            constant_token->vtype_id = UNDEFINED;
            constant_token->value = NULL;
            expression_token = append(expression_token, EXP_TK, constant_token);
        } else if (is_in(*literal, EXPRESSION_TERMINATE_OPERATORS)) {
            char *operator_literal = cut_operator(literal);
            ExpressionToken *operator_token = malloc(sizeof(ExpressionToken));
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
        stack_tmp[tmp_counter++] = symbol;
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

char * cut_string(Array ** exp_tokens) {
    ExpressionToken * token; char * string = NULL; char stack_tmp[255] ="\0"; _next++;
    while (token = get_curr_exp_token(exp_tokens)) {
        if (token->type_id != OP_QUOTE) {
            strcat(stack_tmp, token->literal);
            exp_token_destructor(token);
            token = pop_next_exp_token(exp_tokens);
        } else {
            exp_token_destructor(pop_next_exp_token(exp_tokens));
            break;
        }
    }
    string = alloc_string(stack_tmp);
    return string;
}

/* ROLLBACK SECTION */

//char * cut_string(Array ** exp_tokens) {
//    char stack_tmp[EXPRESSION_MAX_LEN + 1] = "\0";
//    ExpressionToken * token = get_curr_exp_token(exp_tokens);
//    while (token && token->type_id != OP_QUOTE) {
//        ExpressionToken * inner_token = pop_next_exp_token(exp_tokens);
//        strcat(stack_tmp, inner_token->literal);
//        exp_token_destructor(inner_token); // after copy the literal free the pop-up token
//        token = get_curr_exp_token(exp_tokens);
//    }
//    exp_token_destructor(pop_next_exp_token(exp_tokens)); // just pop up and free the quote mark
//
//    char * string_literal = alloc_string(stack_tmp);
//    return string_literal;
//}
//
//Array ** cut_while(Array ** exp_tokens, char * cut_factor) {
//    Array ** tokens = new_array(); ExpressionToken * token;
//    while (token = get_curr_exp_token(exp_tokens)) {
//        char * literal = token->literal;
//        if (!is_in(*literal, cut_factor)) {
//            token = pop_next_exp_token(exp_tokens);
//            tokens = append(tokens, EXP_TK, token);
//        }
//        else {
//            exp_token_destructor(pop_next_exp_token(exp_tokens)); // just free stop_token
//            break;
//        }
//    }
//    return tokens;
//}
//
//Array ** cut_array(Array ** exp_tokens) {
//    Array ** _array = new_array();
//    ExpressionToken * token;
//
//    int o = 1; int c = 0;
//    while (token = get_curr_exp_token(exp_tokens)) {
//        if (token->type_id != OP_COMA) {
//            if (token->type_id == OP_OPEN_BBRACK) {
//                _next++;
//                _array = append(_array, ARRAY, cut_array(exp_tokens));
//            } else _array = append(_array, ARRAY, cut_while(exp_tokens, EXPRESSION_ARRAY_CUT_FACTOR));
//        } else if (is_empty(_array)) throw_arithmetical_exception(token->literal, EXPRESSION_EXPECTED_VALUE);
//    }
//    return _array;
//}

//Array ** cut_array(Array ** exp_tokens) {
//    Array ** _array = new_array();
//    ExpressionToken * token = get_curr_exp_token(exp_tokens);
//    int o_counter = 1; int c_counter = 0;
//    while (token) {
//        switch (token->type_id) {
//            case OP_OPEN_BBRACK:
//                o_counter++;
//                break;
//            case OP_CLOSE_BBRACK:
//                c_counter++;
//                break;
//        }
//        if (o_counter == c_counter) break;
//
//        token = pop_next_exp_token(exp_tokens);
//        if (token->type_id != OP_COMA) _array = append(_array, EXP_TK, token);
//        else exp_token_destructor(token);
//        token = get_curr_exp_token(exp_tokens);
//    }
//    exp_token_destructor(pop_next_exp_token(exp_tokens)); // just pop up end free the }
//
//    return _array;
//}

//void token_typecast(Array ** exp_tokens) {
//    _next = 0; // reset the counter for tokens
//    ExpressionToken * token;
//    while (token = get_next_exp_token(exp_tokens)) {
//        if (token->type_id == OP_QUOTE) {
//            int position = _next - 1;
//            exp_token_destructor(exp_tokens[position]->element);
//            char * string_literal = cut_string(exp_tokens);
//
//            ExpressionToken * string_tk = malloc(sizeof(ExpressionToken));
//            string_tk->type_id = EXPRESSION_CONSTANT_TK;
//            string_tk->literal = NULL;
//            string_tk->vtype_id = STRING;
//            string_tk->value = string_literal;
//            exp_tokens[position]->element = string_tk;
//        } else allocate_token_value(token);
//    }
//
//    // after the all simple type is ready necessary start typecasting for arrays
//    _next = 0; // reset the counter
//    while (token = get_next_exp_token(exp_tokens)) {
//        if (token->type_id == OP_OPEN_BBRACK) {
//            int position = _next - 1;
//            exp_token_destructor(exp_tokens[position]->element);
//            Array ** array = cut_array(exp_tokens);
//            display_array(array);
//            _next = position + 1; // return counter to above state
//
//            ExpressionToken * array_tk = malloc(sizeof(ExpressionToken));
//            array_tk->type_id = EXPRESSION_CONSTANT_TK;
//            array_tk->literal = NULL;
//            array_tk->vtype_id = ARRAY;
//            array_tk->value = array;
//            exp_tokens[position]->element = array_tk;
//        }
//    }
//}

/* ROLLBACK SECTION */

void token_typecast(Array ** exp_tokens) {
    ExpressionToken * token;
    while (token = get_curr_exp_token(exp_tokens)) {
        if (token->type_id == OP_QUOTE) {
            int position = _next;
            char * string = cut_string(exp_tokens);
            ExpressionToken * string_tk = malloc(sizeof(ExpressionToken));
            string_tk->type_id = EXPRESSION_CONSTANT_TK;
            string_tk->literal = NULL;
            string_tk->vtype_id = STRING;
            string_tk->value = string;

            exp_token_destructor(exp_tokens[position]->element); // free the current quote-token
            exp_tokens[position]->element = string_tk;
        } else if (token->type_id == OP_OPEN_BBRACK) {
            int position = _next;
            Array ** array = cut_array(exp_tokens);
            ExpressionToken * array_tk = malloc(sizeof(ExpressionToken));
            array_tk->type_id = EXPRESSION_CONSTANT_TK;
            array_tk->literal = NULL;
            array_tk->vtype_id = ARRAY;
            array_tk->value = array;

            exp_token_destructor(exp_tokens[position]->element); // free the current {-token =
            exp_tokens[position]->element = array_tk;
        } else {
            allocate_token_value(token);
            _next++;
        }
    }
}

int allocate_token_value(ExpressionToken * exp_token) {
    char * literal = exp_token->literal; void * (*function_pointer)(void *, void *) = NULL;
    if (exp_token->value != NULL) return 0;
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
        exp_token->value = function_pointer;
        exp_token->vtype_id = FUNCTION;
    } else if (is_name(literal)) {
        // getting value from name space ???
        exp_token->value = NULL;
        exp_token->vtype_id = UNDEFINED;
    } else {
        throw_arithmetical_exception(literal, EXPRESSION_UNDEFINED_OPERATOR);
        exp_token->value = NULL;
        exp_token->vtype_id = UNDEFINED;
    }
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
        if (is_name = isdigit(*literal) || isalpha(*literal)) break;
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
        case INTEGER: case FLOAT: case STRING:
            free(token->value);
            break;
        case ARRAY:
            array_destructor(token->value);
            break;
    }

    free(token->literal);
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
        case OP_QUOTE: return OP_QUOTE;

        case OP_PLUS: return OP_PLUS;
        case OP_MINUS: return OP_MINUS;
        case OP_MUL: return OP_MUL;
        case OP_DIV: return OP_DIV;

        case OP_COMA: return OP_COMA;
        default: return EXPRESSION_OPERATOR_TK;
    }
}