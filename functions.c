#include "functions.h"

Node * performing_local_namespace(Array ** input_arguments, Function * function_object) {
    Node * root = NULL;
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
    Array ** function_call_arglist = function_call->arg_list;
    Array ** function_object_arglist = function_object->arg_list;
    if (_get_len(function_call_arglist) != _get_len(function_object_arglist)) return 0;

    return 1;
}