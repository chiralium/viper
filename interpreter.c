#include "interpreter.h"
#include "expression.h"
#include "composer.h"
#include "functions.h"
#include "memory.h"
#include "display.h"

extern Array ** call_stack;
extern Array ** memory_table;

/*
 * Entire simple data should be stored in NameSpace,
 * entire complex data stored into heap. NameSpace just stored the pointers to this data.
 * Heap freed by garbage_destructor.
 */

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

    free(result);
    namespace_destructor(global_namespace);

    garbage_destructor(memory_table);
    display_memory_table(memory_table);
    return result;
}

Constant * interpreter(Array ** code, Node * current_namespace) {
    int code_counter = 0; Constant * result = NULL;
    while (code[code_counter]) {
        if (code[code_counter]->type_id == ARRAY) {
            // if this condition is true, then this element is a expression
            result = calculate_expression(code[code_counter]->element, current_namespace);
            display_callstack(call_stack);
            set_color_scheme(COLOR_SCHEME_INTERPRETER_OUTPUT);
            printf(">>> ");
            set_color_scheme(-1);
            display_constant(result); printf("\n");
            (is_simple_data(result->type_id)) ? free(result->value) : NULL;
            free(result); result = NULL;
            free(code[code_counter]);
        } else if (code[code_counter]->type_id == STMT_IF) {
            If * if_statement = code[code_counter]->element;
            if_statement_exec(if_statement, current_namespace);
            if_destructor(if_statement); free(code[code_counter]);
        } else if (code[code_counter]->type_id == STMT_WHILE) {
            free(code[code_counter]->element);
            free(code[code_counter]);
        } else if (code[code_counter]->type_id == STMT_NAMESPACE) {
            NameSpace * namespace_stmt = code[code_counter]->element;
            result = namespace_exec(namespace_stmt);
            namespace_declaration(result->value, current_namespace);
            (is_simple_data(result->type_id)) ? free(result->value) : NULL;
            free(result); result = NULL;
            free(code[code_counter]);
        } else if (code[code_counter]->type_id == STMT_FUNC) {
            Function * function_object = code[code_counter]->element;
            function_declaration(function_object, current_namespace);
            free(code[code_counter]); result = NULL;
        } else if (code[code_counter]->type_id == STMT_RETURN) {
            Return * return_statement = code[code_counter]->element;
            if (!is_function_state()) throw_statement_exception(return_statement->expression, INTERPRETER_INVALID_RETURN_STATEMENT);
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

/* The calculated namespace must be declared into current namespace by his name */
void namespace_declaration(NameSpaceObject * namespace, Node * current_namespace) {
    char * namespace_name = namespace->name;
    Node * namespace_node = find_node(current_namespace, faq6(namespace_name)); Constant * node_value = (namespace_node != NULL) ? namespace_node->value : NULL;
    if (namespace_node == NULL || node_value->type_id != NAMESPACE) {
        if (node_value != NULL) {
            if (is_simple_data(node_value->type_id)) constant_destructor(node_value);
            else free(node_value);
        }
        Constant * namespace_constant = new_constant(NAMESPACE, namespace);
        Node * namespace_node = new_node(faq6(namespace->name), namespace_constant);
        memory_table = append(memory_table, MEMORY_ELEMENT, new_memory_element(NAMESPACE, namespace, "interpreter.c"));
        insert_node(current_namespace, namespace_node);
    } else {
        throw_warning_message(namespace_name, NAMESPACE_REDEFINED_WARNING);
        Constant * node_value = namespace_node->value;
        NameSpaceObject * old_object = node_value->value;
        namespace_destructor(old_object->namespace);
        old_object->namespace = namespace->namespace;
        free(namespace->name);
        free(namespace);
    }
}

/* The function will be execute the namespace body and store the namespace */
Constant * namespace_exec(NameSpace * namespace_stmt) {
    call_stack = append(call_stack, CALLSTACK_POINT, new_call_stack_point(namespace_stmt->name, INTERPRETER_CALL_STACK_NAMESPACE));
    Array ** namespace_code = namespace_stmt->body;
    /* parsing the namespace code */
    Array ** tokens = lexer(namespace_code);
    Array ** parsed_tokens = parser(tokens);
    Array ** expression_tokens = expression_lexer(parsed_tokens);
    composer(expression_tokens);
    array_destructor(namespace_code); array_destructor(tokens);

    Node * local_namespace = new_node(faq6("__name__"), new_constant(STRING, alloc_string(namespace_stmt->name)));
    interpreter(expression_tokens, local_namespace);

    NameSpaceObject * calculated_namespace_object = new_namespace_object(alloc_string(namespace_stmt->name), local_namespace);
    Constant * namespace = new_constant(NAMESPACE, calculated_namespace_object);

    Array * last_call = pop_last_el(call_stack); free(last_call->element); free(last_call);

    free(namespace_stmt->name); free(namespace_stmt);
    return namespace;
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

    /* if the function have no namespace, set it as default */
    if (function_object->namespace == NULL) function_object->namespace = current_namespace;
    else {
        /* getting namespace object by name */
        Node * namespace_object = find_node(current_namespace, faq6(function_object->namespace));
        if (namespace_object == NULL) throw_arithmetical_exception(function_object->namespace, ARITHMETICA_UNDEFINED_NAME);
        else {
            Constant * node_value = namespace_object->value;
            if (node_value->type_id != NAMESPACE) throw_function_exception(function_object->namespace, FUNCTIONS_INVALID_NAMESPACE_TYPE, function_object->name);
            NameSpaceObject * specified_namespace = node_value->value;
            free(function_object->namespace); function_object->namespace = specified_namespace->namespace;
        }
    }

    char * function_name = function_object->name;

    /* find the function container by name */
    Node * function_node = find_node(current_namespace, faq6(function_name)); Constant * node_value = (function_node != NULL) ? function_node->value : NULL ;
    if (function_node == NULL || node_value->type_id != FUNCTION_CONTAINER) {
        /* if the node is an simple variable */
        if (node_value != NULL) {
            if (is_simple_data(node_value->type_id)) constant_destructor(node_value);
            else free(node_value);
        }

        /* if the container is not exist */
        int signature = get_function_signature(function_object->arg_list); // calculate the function signature
        Node * function = new_node(signature, function_object); // create node with function by his signature
        FunctionContainer * function_container = new_function_container(alloc_string(function_name), function, function_object->namespace); // create container with a function
        insert_node(current_namespace, new_node(faq6(function_name), new_constant(FUNCTION_CONTAINER, function_container))); // insert the container into current namespace
        memory_table = append(memory_table, MEMORY_ELEMENT, new_memory_element(FUNCTION_CONTAINER, function_container, "interpreter.c")); // add container to a memory table
        memory_table = append(memory_table, MEMORY_ELEMENT, new_memory_element(FUNCTION, function_object, "interpreter.c")); // add the function to a memory table
    } else {
        /* if the container is already exist */
        Constant * node_value = function_node->value; FunctionContainer * function_container = node_value->value;
        Node * overloaded_function_tree = function_container->overloaded_functions;
        int signature = get_function_signature(function_object->arg_list); Node * overloaded_functions = NULL;
        if ( (overloaded_functions = find_node(overloaded_function_tree, signature)) == NULL ) {
            /* if it is a new function */
            throw_warning_message(function_name, FUNCTIONS_OVERLOADING_WARNING);
            Node * function = new_node(signature, function_object); insert_node(function_container->overloaded_functions, function); // add a new function to a container
            memory_table = append(memory_table, MEMORY_ELEMENT, new_memory_element(FUNCTION, function_object, "interpreter.c")); // add a new function to a memory table
        } else {
            /* if the function with this signature is exist */
            throw_warning_message(function_name, FUNCTIONS_REDEFINITION_WARNING);
            Function * old_function = overloaded_functions->value;
            /* destroy the old function */
            array_destructor(old_function->arg_list);
            array_destructor(old_function->body);
            free(old_function->name);

            /* set a new function to exist pointer */
            old_function->name = function_object->name;
            old_function->arg_list = function_object->arg_list;
            old_function->body = function_object->body;
            old_function->namespace = function_object->namespace;
            free(function_object);
        }
    }
}

/* The main entry point of local function */
Constant * function_exec(Array ** function_code, Node * local_namespace) {
    Constant * returned_value;
    composer(function_code);
    returned_value = interpreter(function_code, local_namespace); (returned_value == NULL) ? returned_value = new_constant(NONE, NULL) : NULL;
    Array * last_call = pop_last_el(call_stack); free(last_call->element); free(last_call);
    namespace_destructor(local_namespace);
    return returned_value;
}

/* The entry point of return statement */
Constant * return_exec(char * return_expression, Node * local_namespace) {
    call_stack = append(call_stack, CALLSTACK_POINT, new_call_stack_point("return", INTERPRETER_CALL_STACK_RETURN));

    Constant * result;
    Array ** literals = recursive_descent(return_expression); free(return_expression);
    Array ** tokens = lexer(literals);
    Array ** parsed_tokens = parser(tokens);
    Array ** expression_tokens = expression_lexer(parsed_tokens);

    composer(expression_tokens);

    result = calculate_expression(expression_tokens[0]->element, local_namespace);

    display_callstack(call_stack);
    set_color_scheme(COLOR_SCHEME_INTERPRETER_OUTPUT);
    printf(">>> ");
    set_color_scheme(-1);
    display_constant(result);
    printf("\n");

    free(expression_tokens[0]); free(expression_tokens);
    array_destructor(literals); array_destructor(tokens);

    Array * last_call = pop_last_el(call_stack); free(last_call->element); free(last_call);
    return result;
}

Constant * if_condition_exec(char * condition, Node * current_namespace) {
    Constant * result;
    Array ** literals = recursive_descent(condition);
    Array ** tokens = lexer(literals);
    Array ** parsed_tokens = parser(tokens);
    Array ** expression_tokens = expression_lexer(parsed_tokens);

    composer(expression_tokens);

    result = calculate_expression(expression_tokens[0]->element, current_namespace);
    free(expression_tokens[0]); free(expression_tokens);
    array_destructor(literals); array_destructor(tokens);
    return result;
}

void if_body_exec(Array ** code, Node * current_namespace) {
    Array ** tokens = lexer(code);
    Array ** parsed_tokens = parser(tokens);
    Array ** expression_tokens = expression_lexer(parsed_tokens);

    composer(expression_tokens);

    interpreter(expression_tokens, current_namespace);
    array_destructor(tokens);
}

void if_statement_exec(If * statement, Node * current_namespace) {
    if (statement->condition != NULL) {
        char * condition = alloc_string(statement->condition);
        Constant * condition_value = if_condition_exec(statement->condition, current_namespace); int flag = *(int *)condition_value->value;
        if (!is_simple_data(condition_value->type_id)) throw_statement_exception(condition, INTERPRETER_INVALID_IF_STATEMENT_VALUE);
        free(condition); constant_destructor(condition_value);
        if (flag) if_body_exec(statement->body, current_namespace);
        else if (statement->else_condition != NULL) if_statement_exec(statement->else_condition, current_namespace);
    } else if_body_exec(statement->body, current_namespace);
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

CallStackPoint * new_call_stack_point(char * label, char type) {
    CallStackPoint * point = malloc(sizeof(CallStackPoint));
    point->label = label;
    point->point_type = type;
    return point;
}

int is_function_state() {
    CallStackPoint * last_point = get_last_el(call_stack)->element;
    return last_point->point_type == INTERPRETER_CALL_STACK_FUNCTION;
}