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
            Function *function_statement = get_function_statement(tokens);
            parsed_tokens = append(parsed_tokens, STMT_FUNC, function_statement);
        } else if (token->type_id == LEXER_RETURN_TK) {
            Return * return_statement = malloc(sizeof(Return));
            token = next_token(tokens);
            return_statement->expression = alloc_string(token->value);
            parsed_tokens = append(parsed_tokens, STMT_RETURN, return_statement);
        } else if (token->type_id == LEXER_NAMESPACE_TK) {
            NameSpace * namespace_statement = get_namespace_statement(tokens);
            parsed_tokens = append(parsed_tokens, STMT_NAMESPACE, namespace_statement);
        } else if (token->type_id == LEXER_EXPRESSION_TK) {
            parsed_tokens = append(parsed_tokens, TOKEN, token);
        } else throw_statement_exception("<complex>", PARSER_COMMON_SYNTAX_EXCEPTION);
    }
    _next = 0;
    return parsed_tokens;
}

NameSpace * get_namespace_statement(Array ** tokens) {
    char * name; Array ** body;

    Token * token = next_token(tokens);

    if (token && token->type_id == LEXER_KEYWORD_PARAM_TK) {
        name = alloc_string(trim(token->value));
        if (!is_statement_name_valid(name)) throw_statement_exception(name, PARSER_INVALID_STATEMENT_NAME);
        token = next_token(tokens);
        if (token && token->type_id == LEXER_COMPLEX_TK) body = copy_array(body, token->value);
        else throw_statement_exception(name, PARSER_MISSING_NAMESPACE_BODY);
    } else throw_statement_exception("namespace", PARSER_MISSING_NAMESPACE_NAME);
    NameSpace * namespace_statement = make_namespace(name, body);
    return namespace_statement;
}

NameSpace * make_namespace(char * name, Array ** body) {
    NameSpace * namespace = malloc(sizeof(NameSpace));
    namespace->name = name;
    namespace->body = body;
    return namespace;
}

If * get_if_statement(Array ** tokens) {
    static int is_else_tail;

    char * condition = NULL; If * else_condition = NULL; Array ** body;

    Token * token = next_token(tokens);

    if (token && token->type_id == LEXER_KEYWORD_PARAM_TK) {
        condition = (trim(token->value) == NULL) ? NULL : alloc_string(trim(token->value));
        token = next_token(tokens);
    } else if (!is_else_tail) throw_statement_exception("if", PARSER_MISSING_IF_CONDITION);
    if (!condition && !is_else_tail) throw_statement_exception("if", PARSER_MISSING_IF_CONDITION);

    if (token && token->type_id == LEXER_COMPLEX_TK) {
        body = copy_array(body, token->value);
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
        condition = (trim(token->value) == NULL) ? NULL : alloc_string(trim(token->value));
        token = next_token(tokens);
        if (token && token->type_id == LEXER_COMPLEX_TK) {
            body = copy_array(body, token->value);
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
    char * name = NULL; char * namespace_name = NULL;
    Array ** body; Array ** arg_list = NULL;

    Token * token = next_token(tokens);

    if (token && token->type_id == LEXER_KEYWORD_PARAM_TK) {
        name = extract_name(token->value);
        namespace_name = extract_namespace_name(name);
        if (!is_statement_name_valid(namespace_name) && namespace_name != NULL) throw_statement_exception(namespace_name, PARSER_INVALID_STATEMENT_NAME);
        if (!is_statement_name_valid(name)) throw_statement_exception(name, PARSER_INVALID_STATEMENT_NAME);
        arg_list = extract_args(token->value);
        token = next_token(tokens);
        if (token && token->type_id == LEXER_COMPLEX_TK) {
            body = copy_array(body, token->value);
        } else throw_statement_exception(name, PARSER_MISSING_FUNC_BODY);
    } else throw_statement_exception("function", PARSER_MISSING_FUNC_PARAMS);
    Function * function_statement = make_function(namespace_name, name, arg_list, body);
    return function_statement;
}

Function * make_function(char * namespace_name, char * name, Array ** arg_list, Array ** body) {
    Function * function_statement = malloc(sizeof(Function));
    function_statement->name = name;
    function_statement->body = body;
    function_statement->arg_list = arg_list;
    function_statement->namespace = namespace_name;
    return function_statement;
}

Array ** extract_args(char * literal) {
    Array ** arg_list = new_array();

    if (pop_first(literal) != OP_OPEN_CBRACK) return NULL; // if the next symbol is not a '(', that means function does'nt have arguments
    if (pop_last(literal) != OP_CLOSE_CBRACK) throw_statement_exception(literal, PARSER_MISSING_FUNC_ARGS_BRACKET);

    if (!*literal) return arg_list; // the empty brackets is the way too to declare none-arguments function

    while (*literal) {
        char * arg = trim(cut_token(literal, OP_COMA));
        if (!*arg) throw_statement_exception(literal, PARSER_MISSING_FUNC_ARGS);
        arg_list = append(arg_list, STRING, arg);
        pop_first(literal);
    }
    if (is_duplicated_args(arg_list)) throw_statement_exception("<function>", PARSER_DUPLICATED_FUNC_ARGS);
    if (*arg_list) return arg_list;
    else return 0;
}

char * extract_name(char * literal) {
    literal = trim(literal);
    char * name = cut_token(literal, OP_OPEN_CBRACK);
    if (*name) return name;
    else throw_statement_exception("function", PARSER_MISSING_FUNC_NAME);
}

char * extract_namespace_name(char * literal) {
    char * namespace_and_function_name = alloc_string(literal);
    char * name = cut_token(namespace_and_function_name, OP_SPACE);
    if (*namespace_and_function_name) {
        free(cut_token(literal, OP_SPACE));
        literal = trim(literal);
        free(namespace_and_function_name);
        return name;
    } else {
        free(namespace_and_function_name);
        free(name);
        return NULL;
    }
}

int is_duplicated_args(Array ** arg_list) {
    int args_count = _get_len(arg_list);
    if (args_count > PARSER_MAX_FUNCTION_ARGS) throw_statement_exception("<function>", PARSER_TOO_MUCH_FUNC_ARGS);
    else if (args_count == 0) return 1;

    /* get faq6 hash for each arg name and create the array */
    int hash[PARSER_MAX_FUNCTION_ARGS];
    for (int i = 0; i < args_count; i++) hash[i] = faq6(arg_list[i]->element);

    /* simple bubble sorting */
    for (int j = 0; j < args_count - 1; j++) {
        int tmp = 0;
        for (int i = 0; i < (args_count - j - 1); i++) {
            if (hash[i] > hash[i + 1]) {
               tmp = hash[i];
               hash[i] = hash[i + 1];
               hash[i + 1] = tmp;
            }
        }
    }

    /* checking for duplicate hashes */
    int i;
    for (i = 1; i < args_count; i++) {
        if (hash[i - 1] == hash[i]) return 1;
    }
    return 0;
}

int is_statement_name_valid(char * name) {
    if (name == NULL) return 0;
    int is_name = 0;
    if (isdigit(*name)) return 0;
    while (*name) {
        if (!(is_name = isdigit(*name) || isalpha(*name) || *name == '_')) break;
        name++;
    }
    return is_name;
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
    array_destructor(statement->body);
    free(statement->name);
    free(statement);
}

void if_destructor(If * statement) {
    if (statement->else_condition) if_destructor(statement->else_condition);
    free(statement->condition);
    array_destructor(statement->body);
    free(statement);
}

void while_destructor(While * statement) {
    array_destructor(statement->body);
    free(statement->condition);
    free(statement);
}

void return_destructor(Return * statement) {
    free(statement->expression);
    free(statement);
}

void namespace_destructor_stmt(NameSpace * statement) {
    array_destructor(statement->body);
    free(statement->name);
    free(statement);
}

Return * copy_return(Return * return_statement) {
    Return * copied_statement = malloc(sizeof(Return));
    copied_statement->expression = alloc_string(return_statement->expression);
    return copied_statement;
}

NameSpace * copy_namespace_statement(NameSpace * namespace_statement) {
    NameSpace * copied_statement = malloc(sizeof(NameSpace));
    copied_statement->name = alloc_string(namespace_statement->name);
    copied_statement->body = copy_array(copied_statement->body, namespace_statement->body);
    return copied_statement;
}

Function * copy_function(Function * function_statement) {
    Function * copied_statement = malloc(sizeof(Function));
    copied_statement->name = alloc_string(function_statement->name);
    copied_statement->namespace = (function_statement->namespace != NULL)  ? alloc_string(function_statement->namespace) : NULL;
    copied_statement->arg_list = copy_array(copied_statement->arg_list, function_statement->arg_list);
    copied_statement->body = copy_array(copied_statement->body, function_statement->body);
    return copied_statement;
}

If * copy_if(If * if_statement) {
    If * copied_statement = malloc(sizeof(If));
    copied_statement->condition = alloc_string(if_statement->condition);
    copied_statement->body = copy_array(copied_statement->body, if_statement->body);
    if (if_statement->else_condition != NULL) copied_statement->else_condition = copy_if(if_statement->else_condition);
    return copied_statement;
}