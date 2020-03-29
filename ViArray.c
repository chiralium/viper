#include "ViArray.h"
#include "exception.h"
#include "types.h"

extern char * expression_as_string;

Constant * _get_by_index(Constant * object, Array ** params) {
    _validate_index_parameter(params);
    int params_count = _get_len(params);
    Constant * result = malloc(sizeof(Constant));

    switch(params_count) {
        case 1:
            if (object->type_id == STRING) {
                int index = *(int *)(params[0]->element); int length = strlen(object->value) - 1;
                if (index > length) throw_arithmetical_exception(expression_as_string, VIARRAY_RANGE_EXCEPTION);
                char * single_symbol = malloc(sizeof(char) * 2);
                single_symbol[0] = ((char *)(object->value))[index];
                single_symbol[1] = '\0';
                result->value = single_symbol;
                result->type_id = STRING;
            } else if (object->type_id == ARRAY) {
                int index = *(int *)(params[0]->element);
                Array * element = ((Array **)(object->value))[index];
                result->value = copy_data(element->element, element->type_id);
                result->type_id = element->type_id;
            }
    }
    constant_destructor(object);
    return result;
}

void _validate_index_parameter(Array ** params) {
    while (*params) {
        Array * parameter = *params;
        switch (parameter->type_id) {
            case INTEGER:
                if (*((int *)parameter->element) < 0) throw_arithmetical_exception(expression_as_string, VIARRAY_INDEX_EXCEPTION);
                break;
            default: throw_arithmetical_exception(expression_as_string, VIARRAY_INDEX_EXCEPTION);
        }
        params++;
    }
}