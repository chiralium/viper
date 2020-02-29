#include "lexer.h"

char * cut_expression(char * literal) {
    char stack_tmp[LEXER_MAX_VALUE + 1]; char symbol;
    int tmp_counter = 0;
    while (symbol = pop_up(literal)) stack_tmp[tmp_counter++] = symbol;
    stack_tmp[tmp_counter] = '\0';
    char * expression_value = alloc_string(stack_tmp);
    return expression_value;
}

char * cut_keyword(char * literal) {
    char stack_tmp[LEXER_MAX_VALUE + 1]; char symbol;
    int tmp_counter = 0;
    while (*literal) {
        if (!is_special(*literal)) {
            symbol = pop_up(literal);
            stack_tmp[tmp_counter++] = symbol;
        }
        else break;
    }
    stack_tmp[tmp_counter] = '\0';
    char * keyword_token = alloc_string(stack_tmp);
    return keyword_token;
}

Array ** lexer(Array ** literals) {
    Array ** tokens = (Array **)calloc(LEXER_MAX_TOKENS, sizeof(Token *));
    int literal_counter = 0;
    while (literals[literal_counter]) {
        /* if the literal is an array it means that literals is a structure complex token, which contained the other types of tokens */
        if (literals[literal_counter] -> type_id != ARRAY) {
            char *literal = (char *) (literals[literal_counter]->element);
            while (*literal) {
                /* Check is the first symbol of literal is an alphabetical symbol */
                if (!is_special(*literal) && !isdigit(*literal)) {
                    char *potential_keyword_tk = cut_keyword(literal);
                    Token *token = (Token *) malloc(sizeof(Token));
                    token->value = potential_keyword_tk;
                    if (is_keyword(potential_keyword_tk)) token->type_id = LEXER_KEYWORD_TK;
                    else token->type_id = LEXER_EXPRESSION_TK;
                    tokens = append(tokens, TOKEN, token);
                } else {
                    /* Else, if the first symbol is not alphabetical it mean the token is a expression */
                    char *expression_tk = cut_expression(literal);
                    Token *token = (Token *) malloc(sizeof(Token));
                    token->value = expression_tk;
                    token->type_id = LEXER_EXPRESSION_TK;
                    tokens = append(tokens, TOKEN, token);
                }
            }
        } else {
            Token *token = (Token *)malloc(sizeof(Token));
            token->value = literals[literal_counter] -> element;
            token->type_id = LEXER_COMPLEX_TK;
            tokens = append(tokens, COMPLEX_TOKEN, token);
        }
        literal_counter++;
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
    if (strcmp(literal, KW_FUNCTION) == 0) return 1;
    else if (strcmp(literal, KW_IF) == 0) return 1;
    else if (strcmp(literal, KW_ELSE) == 0) return 1;
    else if (strcmp(literal, KW_FOR) == 0) return 1;
    return 0;
}