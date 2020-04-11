#include "functions.h"

int validate_function_call(FuncCall * function_call, Function * function_object) {
    Array ** function_call_arglist = function_call->arg_list;
    Array ** function_object_arglist = function_object->arg_list;
    if (_get_len(function_call_arglist) != _get_len(function_object_arglist)) return 0;

    return 1;
}