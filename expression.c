#include "expression.h"

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
    Array ** expression_token = new_array(); char prev_token_type;
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
            prev_token_type = string_token->type_id;
        } else if (!is_in(*literal, EXPRESSION_TERMINATE_OPERATORS) && !is_in(*literal, EXPRESSION_TERMINATE_BRACKETS)) {
            // this if-statement means the first symbol of literal is not an operator and is not a any kind of bracket
            char * constant_literal = cut_constant(literal);
            ExpressionToken * constant_token = malloc(sizeof(ExpressionToken));
            constant_token->type_id = EXPRESSION_CONSTANT_TK;
            constant_token->literal = constant_literal;
            constant_token->vtype_id = UNDEFINED;
            constant_token->value = NULL;
            expression_token = append(expression_token, EXP_TK, constant_token);
            prev_token_type = constant_token->type_id;
        } else if (is_in(*literal, EXPRESSION_TERMINATE_OPERATORS)) {
            char * operator_literal = cut_operator(literal);
            ExpressionToken * operator_token = malloc(sizeof(ExpressionToken));
            operator_token->type_id = get_token_type(*operator_literal);
            operator_token->literal = operator_literal;
            operator_token->vtype_id = UNDEFINED;
            operator_token->value = NULL;
            expression_token = append(expression_token, EXP_TK, operator_token);
            prev_token_type = operator_token->type_id;
        } else if (is_in(*literal, EXPRESSION_TERMINATE_BRACKETS)) {
            char stack_tmp[2]; stack_tmp[0] = pop_first(literal); stack_tmp[1] = '\0';
            char bracket_type = (is_function_call(prev_token_type, stack_tmp[0]) ?
                                 EXPRESSION_CONSTANT_FUNC_TK :
                                 get_token_type(stack_tmp[0]));

            ExpressionToken *bracket_token = malloc(sizeof(ExpressionToken));
            bracket_token->type_id = bracket_type;
            bracket_token->literal = alloc_string(stack_tmp);;
            bracket_token->vtype_id = SYS;
            bracket_token->value = NULL;
            expression_token = append(expression_token, EXP_TK, bracket_token);
            prev_token_type = bracket_token->type_id;
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

int is_function_call(char prev_token_type, char current_bracket) {
    /*
     * Function call is a open bracket only after ']', ')' & CONSTANT:
     * A[0](x), F()(), F()
     */
    if ( (prev_token_type == OP_CLOSE_CBRACK ||
          prev_token_type == OP_CLOSE_SBRACK ||
          prev_token_type == EXPRESSION_CONSTANT_TK) && current_bracket == OP_OPEN_CBRACK) return 1;
    else return 0;
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
    if (literal == NULL) return 0;
    int is_name = 0;
    if (isdigit(*literal)) return 0;
    while (*literal) {
        if (!(is_name = isdigit(*literal) || isalpha(*literal))) break;
        literal++;
    }
    return is_name;
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

ExpressionToken * copy_expression_token(ExpressionToken * token) {
    ExpressionToken * copied_token = malloc(sizeof(ExpressionToken));
    copied_token->type_id = token->type_id;
    copied_token->vtype_id = token->vtype_id;

    if (token->vtype_id == STRING) copied_token->value = alloc_string(token->value);
    else copied_token->value = NULL;

    copied_token->literal = alloc_string(token->literal);
    return copied_token;
}