#include "functions.h"

FunctionContainer * new_function_container(char * name, Node * functions, Node * namespace) {
    FunctionContainer * function_container = malloc(sizeof(FunctionContainer));
    function_container->name = name;
    function_container->overloaded_functions = functions;
    function_container->namespace = namespace;
    return function_container;
}

void function_container_destructor(FunctionContainer * function_container) {
    free(function_container->name);
    overloaded_function_tree_destructor(function_container->overloaded_functions);
    free(function_container);
}

void overloaded_function_tree_destructor(Node * overloaded_function) {
    if (overloaded_function != NULL) {
        if (overloaded_function->left != NULL) overloaded_function_tree_destructor(overloaded_function->left);
        if (overloaded_function->right != NULL) overloaded_function_tree_destructor(overloaded_function->right);
        free(overloaded_function);
    }
}

Node * performing_local_namespace(Array ** input_arguments, Function * function_object) {
    Node * root = extending(function_object->namespace, NULL);
    if (_get_len(function_object->arg_list) > 0) {
        Array ** arguments = function_object->arg_list; int counter = 0;
        while (input_arguments[counter]) {
            char * arg_name = arguments[counter]->element;
            Node * same_node = find_node(root, faq6(arg_name));
            if (same_node == NULL) {
                Node *node = new_node(faq6(arg_name), input_arguments[counter]->element);
                (root == NULL) ? root = insert_node(root, node) : insert_node(root, node);
            } else {
                Constant * value = same_node->value;
                is_simple_data(value->type_id) ? constant_destructor(value) : free(value);
                same_node->value = input_arguments[counter]->element;
            }
            free(input_arguments[counter]);
            counter++;
        }
    }
    free(input_arguments);
    return root;
}

int validate_function_call(FuncCall * function_call, Function * function_object) {
    Array ** function_call_arglist = function_call->arg_list; Array ** function_object_arglist = function_object->arg_list;

    if (is_empty(function_object_arglist) && is_empty(function_call_arglist)) return 1;
    else if (_get_len(function_call_arglist) == _get_len(function_object_arglist)) return 1;

    return 0;
}

Array ** copy_function_code(Array ** function_code) {
    Array ** copied_code = new_array();
    int counter = 0;
    while (function_code[counter]) {
        if (function_code[counter]->type_id == ARRAY) {
            /* If the element has type array it is means current part of code is an expressions */
            Array ** tokens; tokens = copy_array(tokens, function_code[counter]->element);
            copied_code = append(copied_code, ARRAY, tokens);
        } else if (function_code[counter]->type_id == STMT_NAMESPACE) {
            NameSpace * copied_namespace_statement = copy_namespace_statement(function_code[counter]->element);
            copied_code = append(copied_code, STMT_NAMESPACE, copied_namespace_statement);
        } else if (function_code[counter]->type_id == STMT_RETURN) {
            Return * copied_return_statement = copy_return(function_code[counter]->element);
            copied_code = append(copied_code, STMT_RETURN, copied_return_statement);
        }
        counter++;
    }
    return copied_code;
}

void function_code_destructor(Array ** function_code, int start_position) {
    int counter = start_position;
    while (function_code[counter]) {
        /* if the current element of function code is an array which means this is an expression */
        if (function_code[counter]->type_id == ARRAY) array_destructor(function_code[counter]->element);
        else if (function_code[counter]->type_id == STMT_RETURN) return_destructor(function_code[counter]->element);
        else if (function_code[counter]->type_id == STMT_FUNC) function_destructor(function_code[counter]->element);
        else if (function_code[counter]->type_id == STMT_IF) if_destructor(function_code[counter]->element);
        else if (function_code[counter]->type_id == STMT_WHILE) while_destructor(function_code[counter]->element);
        free(function_code[counter]); counter++;
    }
    free(function_code);
}

int get_function_signature(Array ** arg_list) {
    /* get count arguments as signature */
    int signature = _get_len(arg_list) + 1;
    /* get types of arguments as signature */
    return signature;
}

Function * get_function_from_container(FunctionContainer * function_container, int signature) {
    Node * functions = function_container->overloaded_functions;
    Node * function = find_node(functions, signature);
    return (function != NULL) ? function->value : function;
}