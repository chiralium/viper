#include "parser.h"
#include "display.h"

Array ** parser(Array ** tokens) {
    Array ** parsed_tokens = new_array();
    Token * token;
    while (*tokens) {
        token = (*tokens) -> element;
        if (token -> type_id == LEXER_IF_TK) {
            If * if_statement = cut_if_statement(tokens);
            parsed_tokens = append(parsed_tokens, STMT_IF, if_statement);
        } else if (token -> type_id == LEXER_EXPRESSION_TK) {
            Array * expression_token = pop_el(tokens);
            parsed_tokens = append(parsed_tokens, TOKEN, (Token *)(expression_token -> element));
        } else if (token -> type_id == LEXER_NULL_TK) {
            Array * _for_freed = pop_el(tokens);
            token_destructor(_for_freed->element);
            free(_for_freed);
        }
    }
    parsed_tokens = append(parsed_tokens, NULL_TOKEN, get_null_token());
    return parsed_tokens;
}

If * make_if(char * condition, Array ** body, Array ** else_body) {
    If * if_statement = (If *)malloc(sizeof(If));
    if_statement -> condition = condition;
    if_statement -> body = body;
    if (else_body) if_statement -> else_body = else_body;
    else if_statement -> else_body = NULL;
}

If * cut_if_statement(Array ** tokens) {
    // if pop el returned the token with LEXER_NULL_TK -- throw exception
    pop_el(tokens);
    Array * condition_element = pop_el(tokens);
    char * condition = ((Token *)(condition_element -> element)) -> value;

    Array * body_element = pop_el(tokens); Array ** else_body;
    Array ** body = ((Token *)(body_element -> element)) -> value;

    Token * token = (*tokens) -> element;
    if (token -> type_id == LEXER_ELSE_TK) else_body = cut_else_statement(tokens);
    else else_body = 0;

    If * if_statement = make_if(condition, body, else_body);
    return if_statement;
}

Array ** cut_else_statement(Array ** tokens) {
    pop_el(tokens); pop_el(tokens);

    Array * else_body_element = pop_el(tokens);
    Array ** else_body = ((Token *)(else_body_element -> element)) -> value;

    return else_body;
}

char * trim(char * literal) {
    while (get_first(literal) == FPARSER_SPACE) pop_first(literal); // left side cutter
    while (get_last(literal) == FPARSER_SPACE) pop_last(literal);   // right side cutter
    return literal;
}

void if_destructor(If * statement) {
    array_destructor(statement->body);
    if (statement->else_body) array_destructor(statement->else_body);
    free(statement->condition);
    free(statement);
}

void for_destructor(For * statement) {
    array_destructor(statement->body);
    free(statement->params);
    free(statement);
}

void function_destructor(Function * statement) {
    array_destructor(statement->body);
    array_destructor(statement->arg_list);
    free(statement->name);
    free(statement);
}