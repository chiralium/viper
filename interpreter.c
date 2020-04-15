#include "interpreter.h"
#include "expression.h"

extern Array ** call_stack;

Array ** main_parsing(char * input_stream) {
    Array ** literals = recursive_descent(input_stream); free(input_stream);
    Array ** tokens = lexer(literals);
    Array ** parsed_tokens = parser(tokens);
    Array ** expression_tokens = expression_lexer(parsed_tokens);

    array_destructor(literals);
    array_destructor(tokens);
    return expression_tokens;
}

/* The main entry point of program */
Constant * main_entry(char * input_stream) {
    Constant * result;

    /* parsing the input text stream */
    Array ** parsed = main_parsing(input_stream);

    /* initialization the namespace of interpreter */
    Node * current_namespace = meta_data();

    /* interpreting the program in a current_namespace */
    result = interpreter(parsed, current_namespace);

    (result == NULL) ? result = new_constant(NONE, NULL) : NULL;
    constant_destructor(result); namespace_destructor(current_namespace);
    return result;
}

/* The function will be parse the function code and store it into namespace */
void function_declaration(Function * function_object, Node * current_namespace) {
    Array ** function_code = function_object->body;
    /* parsing the function code */
    Array ** tokens = lexer(function_code);  // extracting simple tokens
    Array ** parsed_tokens = parser(tokens); // extracting complex statements
    Array ** expression_tokens = expression_lexer(parsed_tokens); // extracting tokens from expressions (if exists)
    array_destructor(function_code); array_destructor(tokens);

    /* now, function object has a parsed code */
    function_object->body = expression_tokens;

    /* store the function object with parsed code into namespace by function name */
    char * function_name = function_object->name;
    Constant * node_value = new_constant(FUNCTION, function_object);
    Node * function = new_node(faq6(function_name), node_value);
    insert_node(current_namespace, function);
}

/* The main entry point of local function */
Constant * function_exec(Array ** function_code, Node * local_namespace) {
    Constant * returned_value;
    Array ** tokens = lexer(function_code);
    Array ** parsed_tokens = parser(tokens);
    Array ** expression_tokens = expression_lexer(parsed_tokens);
    returned_value = interpreter(expression_tokens, local_namespace);

    Array * last_call = pop_last_el(call_stack); free(last_call->element); free(last_call);

    array_destructor(function_code); array_destructor(tokens);
    if (returned_value == NULL) {
        namespace_destructor(local_namespace);
        return new_constant(NONE, NULL);
    }else {
        namespace_destructor(local_namespace);
        return returned_value;
    }
}

/* The entry point of return statement */
Constant * return_exec(char * return_expression, Node * local_namespace) {
    call_stack = append(call_stack, STRING, alloc_string("return"));

    Constant * result;
    Array ** literals = recursive_descent(return_expression); free(return_expression);
    Array ** tokens = lexer(literals);
    Array ** parsed_tokens = parser(tokens);
    Array ** expression_tokens = expression_lexer(parsed_tokens);
    result = interpreter(expression_tokens, local_namespace);
    array_destructor(literals); array_destructor(tokens);

    Array * last_call = pop_last_el(call_stack); free(last_call->element); free(last_call);
    return result;
}

Constant * interpreter(Array ** code, Node * current_namespace) {
    int code_counter = 0; Constant * result = NULL;
    while (code[code_counter]) {
        if (result != NULL) constant_destructor(result);
        if (code[code_counter]->type_id == ARRAY) {
            // if this condition is true, then this element is a expression
            result = calculate_expression(code[code_counter]->element, current_namespace);
            printf("\nRUNTIME: "); display_callstack(call_stack); display_constant(result); printf("\n");
            free(code[code_counter]);
        } else if (code[code_counter]->type_id == STMT_IF) {
            if_destructor(code[code_counter]->element);
            free(code[code_counter]);
        } else if (code[code_counter]->type_id == STMT_WHILE) {
            free(code[code_counter]->element);
            free(code[code_counter]);
        } else if (code[code_counter]->type_id == STMT_FUNC) {
            Function * function_object = code[code_counter]->element;
            function_declaration(function_object, current_namespace);
            free(code[code_counter]); result = NULL;
        } else if (code[code_counter]->type_id == STMT_RETURN) {
            Return * return_statement = code[code_counter]->element;
            result = return_exec(return_statement->expression, current_namespace);
            free(code[code_counter]->element);
            free(code[code_counter]);
            break;
        }
        code_counter++;
    }
    free(code);
    return result; // result will be not null only if the code contained return statement
}

Constant * calculate_expression(Array ** expression, Node * current_namespace) {
    Constant * value = arithmetica_wrapper(expression, current_namespace);
    return value;
}

Node * meta_data() {
    char * ver = calloc(sizeof(char), 10); strcpy(ver, "VIPER.v4");
    Constant * version = new_constant(STRING, ver);

    float * pi = malloc(sizeof(float)); *pi = 3.14;
    Constant * pi_number = new_constant(FLOAT, pi);

    Node * root = new_node(faq6("pi"), pi_number);
    Node * ver_node = new_node(faq6("version"), version);

    insert_node(root, ver_node);

    return root;
}

void display_callstack(Array ** points) {
    printf("CallStack: ");
    while (*points) {
        printf("-> %s ", (*points)->element);
        points++;
    }
}
