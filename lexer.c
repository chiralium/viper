#include "lexer.h"
#include "display.h"

char * cut_token(char * literal, char stop_symbol) {
    char stack_tmp[LEXER_MAX_VALUE + 1]; char symbol;
    int tmp_counter = 0;
    while (symbol = get_first(literal)) {
        if (symbol == stop_symbol) break;
        else stack_tmp[tmp_counter++] = pop_first(literal);
    }
    stack_tmp[tmp_counter] = '\0';
    char * token_value = alloc_string(stack_tmp);
    return token_value;
}

Array ** extract_token(char * literal, Array ** tokens) {
    char * token_value = cut_token(literal, OP_SPACE); char token_id;
    if (token_id = is_keyword(token_value)) {
        char * keyword_param = cut_token(literal, '\0');
        Token * keyword_token = (Token *)malloc(sizeof(Token));
        keyword_token->type_id = token_id;
        keyword_token->value = token_value;
        tokens = append(tokens, TOKEN, keyword_token);
        if (*keyword_param) {
            Token *keyword_param_token = (Token *) malloc(sizeof(Token));
            keyword_param_token->type_id = LEXER_KEYWORD_PARAM_TK;
            keyword_param_token->value = keyword_param;
            tokens = append(tokens, TOKEN, keyword_param_token);
        }
    } else {
        char * token_part = cut_token(literal, '\0'); char stack_tmp[LEXER_MAX_VALUE + 1];
        strcpy(stack_tmp, token_value); strcat(stack_tmp, token_part);
        free(token_part); free(token_value);
        token_value = alloc_string(stack_tmp);
        Token * expression_token = (Token *)malloc(sizeof(Token));
        expression_token->type_id = LEXER_EXPRESSION_TK;
        expression_token->value = token_value;
        tokens = append(tokens, TOKEN, expression_token);
    }
    return tokens;
}

Array ** lexer(Array ** literals) {
    Array ** tokens = new_array();
    int literals_counter = 0;
    while (literals[literals_counter]) {
        if (literals[literals_counter] -> type_id != ARRAY) {
            char * literal = (char *)(literals[literals_counter] -> element);
            tokens = extract_token(literal, tokens);
        } else {
            /* It means the literal is a complex token which can contain the other types of tokens */
            Token * complex_token = (Token *)malloc(sizeof(Token));
            complex_token->type_id = LEXER_COMPLEX_TK;
            complex_token->value = literals[literals_counter] -> element;
            tokens = append(tokens, COMPLEX_TOKEN, complex_token);
        }
        literals_counter++;
    }
    return tokens;
}

int is_special(char symbol) {
    switch (symbol) {
        case OP_PLUS:
        case OP_MINUS:
        case OP_MUL:
        case OP_DIV:
        case OP_CLOSE_CBRACK:
        case OP_OPEN_CBRACK:
        case OP_QUOTE:
        case OP_SPACE:
            return 1;
    }
    return 0;
}

int is_keyword(char * literal) {
    if (strcmp(literal, KW_FUNCTION) == 0) return LEXER_FUNCTION_TK;
    else if (strcmp(literal, KW_IF) == 0) return LEXER_IF_TK;
    else if (strcmp(literal, KW_ELSE) == 0) return LEXER_ELSE_TK;
    else if (strcmp(literal, KW_FOR) == 0) return LEXER_FOR_TK;
    else if (strcmp(literal, KW_WHILE) == 0) return LEXER_WHILE_TK;
    return 0;
}

void token_destructor(Token * token) {
    if (token->type_id != COMPLEX_TOKEN) free(token->value);
    else array_destructor(token->value);
    free(token);
}

char get_first(char * literal) {
    return *literal;
}

char get_last(char * literal) {
    if (*literal) {
        int length = strlen(literal) - 1;
        return literal[length];
    } else {
        return 0;
    }
}