#include "ViString.h"
#include "ViArray.h"
#include "exception.h"

extern char * expression_as_string;

Constant * get_char_by_index(Constant * object, Array ** params) {
    int params_count = _get_len(params);
    Constant * result;
    switch (params_count) {
        case 1:
            result = get_single_char(object, params[0]->element);
            break;
        case 2:
            validate_index_parameter(params);
            result = get_substr(object, params[0]->element, params[1]->element);
            break;
        case 3:
            validate_index_parameter(params);
            result = get_substr_step(object, params[0]->element, params[1]->element, params[2]->element);
            break;
    }
    free(object);
    return result;
}

Constant * get_single_char(Constant * object, Constant * index) {
    Constant * element;
    int index_value = *(int *)(index->value);
    char * string = object->value;
    if (strlen(string) <= index_value) throw_arithmetical_exception(expression_as_string, VISTRING_RANGE_EXCEPTION);
    static char stack_tmp[2] = "\0"; stack_tmp[0] = string[index_value];
    element = new_constant(STRING, stack_tmp);
    free(string);
    return element;
}

Constant * get_substr(Constant * object, Constant * start, Constant * end) {
    Constant * sub_element;
    int start_indx = *(int *)(start->value); int end_indx = *(int *)(end->value);
    char * string = object->value;
    if (start_indx >= strlen(string) || end_indx >= strlen(string)) throw_arithmetical_exception(expression_as_string, VISTRING_RANGE_EXCEPTION);
    static char sub_string[ARITHMETICA_MAX_STRING_LEN];
    memcpy(sub_string, string + start_indx, end_indx - start_indx);
    sub_element = new_constant(STRING, sub_string);
    free(string);
    return sub_element;
}

Constant * get_substr_step(Constant * object, Constant * start, Constant * end, Constant * step) {
    Constant * sub_element;
    int start_indx = *(int *)(start->value); int end_indx = *(int *)(end->value); int step_val = *(int *)(step->value);
    char * string = object->value;
    if (start_indx >= strlen(string) || end_indx >= strlen(string)) throw_arithmetical_exception(expression_as_string, VISTRING_RANGE_EXCEPTION);
    static char sub_string[ARITHMETICA_MAX_STRING_LEN];
    int index = 0;
    while (start_indx <= end_indx) {
        sub_string[index++] = string[start_indx];
        start_indx += step_val;
    }
    sub_element = new_constant(STRING, sub_string);
    free(string);
    return sub_element;
}
