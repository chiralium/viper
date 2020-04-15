#include "functions.h"

Node * performing_local_namespace(Array ** input_arguments, Function * function_object) {
    /*
     * create the function object into local namesapce and set not null origin for function object,
     * cause the origin of this object is an parent namespace
     * */
    Constant * function = new_constant(FUNCTION, function_object); function->origin = 1;
    Node * root = new_node(faq6(function_object->name), function);

    /*
     * create the local namespace by input parameters
     */
    if (_get_len(function_object->arg_list) > 0) {
        Array ** arguments = function_object->arg_list;
        int counter = 0;
        while (input_arguments[counter]) {
            char * arg_name = arguments[counter]->element;
            Node * node = new_node(faq6(arg_name), input_arguments[counter]->element);
            (root == NULL) ? root = insert_node(root, node) : insert_node(root, node);
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
        } else if (function_code[counter]->type_id == STMT_RETURN) {
            Return * copied_return_statement = copy_return(function_code[counter]->element);
            copied_code = append(copied_code, STMT_RETURN, copied_return_statement);
        }
        counter++;
    }
    return copied_code;
}