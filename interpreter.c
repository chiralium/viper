#include "interpreter.h"

extern Array ** call_stack;

Constant * main_entry(char * single_line) {
    Constant * result;
    Node * current_namespace = meta_data();
    Array ** literals = recursive_descent(single_line); free(single_line);
    Array ** tokens = lexer(literals);
    Array ** parsed_tokens = parser(tokens);
    Array ** expression_tokens = expression_lexer(parsed_tokens);
    result = interpreter(expression_tokens, current_namespace);
    array_destructor(literals); array_destructor(tokens);
    namespace_destructor(current_namespace);
    return result;
}

void function_declaration(Function * function_object, Node * current_namespace) {
    char * function_name = function_object->name;
    Constant * node_value = new_constant(FUNCTION, function_object);
    Node * function = new_node(faq6(function_name), node_value);
    insert_node(current_namespace, function);
}

Constant * function_exec(Array ** function_code, Node * local_namespace) {
    Constant * returned_value;
    Array ** tokens = lexer(function_code);
    Array ** parsed_tokens = parser(tokens);
    Array ** expression_tokens = expression_lexer(parsed_tokens);
    returned_value = interpreter(expression_tokens, local_namespace);


    if (returned_value == NULL) {
        namespace_destructor(local_namespace);
        return new_constant(NONE, NULL);
    }else {
        namespace_destructor(local_namespace);
        return returned_value;
    }
}

Constant * return_exec(char * return_expression, Node * local_namespace) {
    Constant * result;
    Array ** literals = recursive_descent(return_expression); free(return_expression);
    Array ** tokens = lexer(literals);
    Array ** parsed_tokens = parser(tokens);
    Array ** expression_tokens = expression_lexer(parsed_tokens);
    result = interpreter(expression_tokens, local_namespace);
    array_destructor(literals); array_destructor(tokens);
    return result;
}

Constant * interpreter(Array ** code, Node * current_namespace) {
    printf("\nCallStack: "); display_callstack(call_stack);
    int code_counter = 0; Constant * result = NULL;
    while (code[code_counter]) {
        if (code[code_counter]->type_id == ARRAY) {
            // if this condition is true, then this element is a expression
            Constant * value = calculate_expression(code[code_counter]->element, current_namespace);
            printf("\n"); display_constant(value); printf("\n");
            constant_destructor(value);
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
            free(code[code_counter]);
        } else if (code[code_counter]->type_id == STMT_RETURN) {
            Return * return_statement = code[code_counter]->element;
            call_stack = append(call_stack, STRING, alloc_string("return"));
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
    while (*points) {
        printf("-> %s ", (*points)->element);
        points++;
    }
}
