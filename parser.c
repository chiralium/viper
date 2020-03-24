#include "parser.h"
#include "display.h"

static int _next;

Array ** parser(Array ** tokens) {
    Array ** parsed_tokens = new_array();
    Token * token;
    while (token = next_token(tokens)) {
        if (token->type_id == LEXER_IF_TK) {
            If *if_statement = get_if_statement(tokens);
            parsed_tokens = append(parsed_tokens, STMT_IF, if_statement);
        } else if (token->type_id == LEXER_WHILE_TK) {
            While * while_statement = get_while_statement(tokens);
            parsed_tokens = append(parsed_tokens, STMT_WHILE, while_statement);
        } else if (token->type_id == LEXER_FUNCTION_TK) {
            Function * function_statement = get_function_statement(tokens);
            parsed_tokens = append(parsed_tokens, STMT_FUNC, function_statement);
        } else if (token->type_id == LEXER_EXPRESSION_TK) {
            parsed_tokens = append(parsed_tokens, TOKEN, token);
        } else throw_statement_exception("<complex>", PARSER_COMMON_SYNTAX_EXCEPTION);
    }
    return parsed_tokens;
}

If * get_if_statement(Array ** tokens) {
    static int is_else_tail;

    char * condition = NULL; If * else_condition = NULL; Array ** body;

    Token * token = next_token(tokens);

    if (token && token->type_id == LEXER_KEYWORD_PARAM_TK) {
        condition = trim(token->value);
        token = next_token(tokens);
    } else if (!is_else_tail) throw_statement_exception("if", PARSER_MISSING_IF_CONDITION);
    if (!condition && !is_else_tail) throw_statement_exception("if", PARSER_MISSING_IF_CONDITION);

    if (token && token->type_id == LEXER_COMPLEX_TK) {
        body = token->value;
        // the next token can be ELSE statement, but its not necessary, lets check it
        token = get_token(tokens);
        if (token && token->type_id == LEXER_ELSE_TK) {
            is_else_tail = 1; _next++;
            else_condition = get_if_statement(tokens);
        }
    } else throw_statement_exception(condition, is_else_tail ? PARSER_MISSING_ELSE_BODY : PARSER_MISSING_IF_BODY);

    If * if_statement = make_if(condition, body, else_condition);
    return if_statement;
}

If * make_if(char * condition, Array ** body,  If * else_condition) {
    If * if_statement = malloc(sizeof(If));
    if_statement -> condition = condition;
    if_statement -> body = body;
    if_statement -> else_condition = else_condition;

    return if_statement;
}

While * get_while_statement(Array ** tokens) {
    char * condition = NULL; Array ** body;

    Token * token = next_token(tokens);

    if (token && token->type_id == LEXER_KEYWORD_PARAM_TK) {
        condition = trim(token->value);
        token = next_token(tokens);
        if (token && token->type_id == LEXER_COMPLEX_TK) {
            body = token->value;
            if (!*body) throw_statement_exception(condition, PARSER_MISSING_WHILE_BODY);
        } else throw_statement_exception(condition, PARSER_MISSING_WHILE_BODY);
    }  else throw_statement_exception("while", PARSER_MISSING_WHILE_CONDITION);
    While * while_statement = make_while(condition, body);
    return while_statement;
}

While * make_while(char * condition, Array ** body) {
    While * while_statement = malloc(sizeof(While));
    while_statement->condition = condition;
    while_statement->body = body;
    return while_statement;
}

Function * get_function_statement(Array ** tokens) {
    char * name; Array ** body; Array ** arg_list = NULL;

    Token * token = next_token(tokens);

    if (token && token->type_id == LEXER_KEYWORD_PARAM_TK) {
        name = extract_name(token->value);
        arg_list = extract_args(token->value);
        token = next_token(tokens);
        if (token && token->type_id == LEXER_COMPLEX_TK) {
            body = token->value;
        } else throw_statement_exception(name, PARSER_MISSING_FUNC_BODY);
    } else throw_statement_exception("function", PARSER_MISSING_FUNC_PARAMS);
    Function * function_statement = make_function(name, arg_list, body);
    return function_statement;
}

Function * make_function(char * name, Array ** arg_list, Array ** body) {
    Function * function_statement = malloc(sizeof(Function));
    function_statement->name = name;
    function_statement->body = body;
    function_statement->arg_list = arg_list;

    return function_statement;
}

Array ** extract_args(char * literal) {
    if (pop_first(literal) != OP_OPEN_CBRACK) return NULL; // if the next symbol is not a '(', that means function does'nt have arguments
    if (pop_last(literal) != OP_CLOSE_CBRACK) throw_statement_exception(literal, PARSER_MISSING_FUNC_ARGS_BRACKET);

    if (!*literal) return NULL; // the empty brackets is the way too to declare none-arguments function

    Array ** arg_list = new_array();
    while (*literal) {
        char * arg = trim(cut_token(literal, OP_COMA));
        if (!*arg) throw_statement_exception(literal, PARSER_MISSING_FUNC_ARGS);
        arg_list = append(arg_list, STRING, arg);
        pop_first(literal);
    }
    if (*arg_list) return arg_list;
    else return 0;
}

char * extract_name(char * literal) {
    literal = trim(literal);
    char * name = cut_token(literal, OP_OPEN_CBRACK);
    if (*name) return name;
    else throw_statement_exception("function", PARSER_MISSING_FUNC_NAME);
}

Token * next_token(Array ** tokens) {
    if (tokens[_next]) return (Token *)(tokens[_next++] -> element);
    else return 0;
}

Token * get_token(Array ** tokens) {
    if (tokens[_next]) return (Token *)(tokens[_next] -> element);
    else return 0;
}

char * trim(char * literal) {
    while (get_first(literal) == FPARSER_SPACE) pop_first(literal); // left side cutter
    while (get_last(literal) == FPARSER_SPACE) pop_last(literal);   // right side cutter
    if (!*literal) return NULL;
    return literal;
}

void function_destructor(Function * statement) {
    array_destructor(statement->arg_list);
    free(statement->name);
    free(statement);
}

void if_destructor(If * statement) {
    if (statement->else_condition) if_destructor(statement->else_condition);
    free(statement);
}

void parsed_token_destructor(Array ** parsed_token) {
    int counter = 0;
    while (parsed_token[counter]) {
        if (parsed_token[counter]->type_id == STMT_IF) if_destructor(parsed_token[counter]->element);
        else if (parsed_token[counter]->type_id == STMT_WHILE) free(parsed_token[counter]->element);
        else if (parsed_token[counter]->type_id == STMT_FUNC) function_destructor(parsed_token[counter]->element);
        free(parsed_token[counter]);
        counter++;
    }
    free(parsed_token);
}