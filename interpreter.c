#include "interpreter.h"
#include "expression.h"
#include "composer.h"
#include "functions.h"

extern Array ** call_stack;

Array ** main_parsing(char * input_stream) {
    Array ** literals = recursive_descent(input_stream); free(input_stream);
    Array ** tokens = lexer(literals);
    Array ** parsed_tokens = parser(tokens);
    Array ** expression_tokens = expression_lexer(parsed_tokens);

    composer(expression_tokens); // compose the token list into objects

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
    Node * global_namespace = meta_data();

    /* interpreting the program in a global_namespace */
    result = interpreter(parsed, global_namespace); (result == NULL) ? result = new_constant(NONE, NULL) : NULL;

    constant_destructor(result);
    namespace_destructor(global_namespace);
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
    composer(function_code);
    returned_value = interpreter(function_code, local_namespace); (returned_value == NULL) ? returned_value = new_constant(NONE, NULL) : NULL;

    /* if value having origin, remove node from namespace by origin and free the node value wrapper (Constant *) */
    if (returned_value->origin != NULL) {
        if (is_belonged(local_namespace, returned_value->origin)) {
            free(remove_node(returned_value->origin));
            returned_value->origin = NULL;
        }
    }

    Array * last_call = pop_last_el(call_stack); free(last_call->element); free(last_call);
    namespace_destructor(local_namespace);
    return returned_value;
}

/* The entry point of return statement */
Constant * return_exec(char * return_expression, Node * local_namespace) {
    call_stack = append(call_stack, STRING, alloc_string("return"));

    Constant * result;
    Array ** literals = recursive_descent(return_expression); free(return_expression);
    Array ** tokens = lexer(literals);
    Array ** parsed_tokens = parser(tokens);
    Array ** expression_tokens = expression_lexer(parsed_tokens);

    composer(expression_tokens);

    result = interpreter(expression_tokens, local_namespace);
    array_destructor(literals); array_destructor(tokens);

    Array * last_call = pop_last_el(call_stack); free(last_call->element); free(last_call);
    return result;
}

Constant * interpreter(Array ** code, Node * current_namespace) {
    int code_counter = 0; Constant * result = NULL;
    while (code[code_counter]) {
        if (code[code_counter]->type_id == ARRAY) {
            // if this condition is true, then this element is a expression
            result = calculate_expression(code[code_counter]->element, current_namespace);

            printf("\nRUNTIME: "); display_callstack(call_stack); display_constant(result); printf("\n");

            /* destroy the result if the is not return statement */
            if (!is_return_call(call_stack)) {
                constant_destructor(result);
                result = NULL;
            }

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
            free(code[code_counter]->element); free(code[code_counter]); code_counter++;
            break;
        }
        code_counter++;
    }
    function_code_destructor(code, code_counter);
    return result; // result will be not null only if the code contained return statement OR if the __MAIN__ call
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

int is_main_call(Array ** call_stack) {
    Array * last_call = get_last_el(call_stack);
    char * call_name = last_call->element;
    return strcmp(call_name, "__MAIN__") == 0;
}

int is_return_call(Array ** call_stack) {
    Array * last_call = get_last_el(call_stack);
    char * call_name = last_call->element;
    return strcmp(call_name, "return") == 0;
}

void display_callstack(Array ** points) {
    printf("CallStack: ");
    while (*points) {
        printf("-> %s ", (*points)->element);
        points++;
    }
}

