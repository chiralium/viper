#include "parser.h"
#include "display.h"

Array ** parser(Array ** tokens) {
    Array ** parsed_tokens = new_array();
    Token * token;
    while (token = next_token(tokens)) {
        if (token->type_id == LEXER_IF_TK) {
            If * if_statement = get_if_statement(tokens);
            parsed_tokens = append(parsed_tokens, STMT_IF, if_statement);
        } else if (token->type_id == LEXER_EXPRESSION_TK) {
            parsed_tokens = append(parsed_tokens, TOKEN, token);
        }
    }
    return parsed_tokens;
}

If * get_if_statement(Array ** tokens) {
    char * condition; char * else_condition = NULL;
    Array ** body; Array ** else_body = NULL;

    Token * token = next_token(tokens);

    if (token && token->type_id == LEXER_KEYWORD_PARAM_TK) {
        condition = trim((char *)token->value);
        if (!*condition) throw_statement_exception("if", PARSER_MISSING_IF_CONDITION);
        token = next_token(tokens);
        if (token && token->type_id == LEXER_COMPLEX_TK) {
            body = token->value;
            // the next token can be ELSE statement, but its not necessary, lets check it
            token = next_token(tokens);
            if (token && token->type_id == LEXER_ELSE_TK) {
                token = next_token(tokens);
                if (token && token->type_id == LEXER_KEYWORD_PARAM_TK) {
                    else_condition = (char *)token->value;
                    token = next_token(tokens);
                    if (token && token->type_id == LEXER_COMPLEX_TK) {
                        else_body = token->value;
                    } else throw_statement_exception(else_condition, PARSER_MISSING_ELSE_BODY_MSG);
                } else if (token && token->type_id == LEXER_COMPLEX_TK) else_body = token->value; else throw_statement_exception("else", PARSER_MISSING_ELSE_BODY_MSG);
            }
        } else throw_statement_exception(condition, PARSER_MISSING_IF_BODY_MSG);
    } else throw_statement_exception("if", PARSER_MISSING_IF_CONDITION);
    If * if_statement = make_if(condition, body, else_condition, else_body);
    return if_statement;
}

If * make_if(char * condition, Array ** body, char * else_condition, Array ** else_body) {
    If * if_statement = malloc(sizeof(If));
    if_statement -> condition = condition;
    if_statement -> body = body;

    if (else_body) {
        if_statement->else_condition = else_condition;
        if_statement->else_body = else_body;
    }
    return if_statement;
}

Token * next_token(Array ** tokens) {
    static int _next;
    if (tokens[_next]) return (Token *)(tokens[_next++] -> element);
    else return 0;
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

void parsed_token_destructor(Array ** parsed_token) {
    int counter = 0;
    while (parsed_token[counter]) {
        if (parsed_token[counter]->type_id == STMT_IF) free(parsed_token[counter]->element);
        free(parsed_token[counter]);
        counter++;
    }
    free(parsed_token);
}