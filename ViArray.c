#include "ViArray.h"
#include "exception.h"
#include "types.h"
#include "fread.h"

extern char * expression_as_string;

Constant * _get_by_index(Constant * object, Array ** params) {
    _validate_index_parameter(params);
    int params_count = _get_len(params);
    Constant * result;

    switch(params_count) {
        case 1:
            result = _get_single(object, *(int *)(params[0]->element));
            break;
        case 2:
            result = _get_range(object, *(int *)(params[0]->element), *(int *)(params[1]->element));
            break;
        case 3:
            result = _get_range_step(object, *(int *)(params[0]->element), *(int *)(params[1]->element), *(int *)(params[2]->element));
            break;
    }
    constant_destructor(object);
    return result;
}

Constant * _get_single(Constant * object, int index) {
    Constant * result = malloc(sizeof(Constant));
    if (object->type_id == STRING) {
        int length = strlen(object->value) - 1;
        if (index > length) throw_arithmetical_exception(expression_as_string, VIARRAY_RANGE_EXCEPTION);
        char * single_symbol = malloc(sizeof(char) * 2);
        single_symbol[0] = ((char *)(object->value))[index];
        single_symbol[1] = '\0';
        result->value = single_symbol;
        result->type_id = STRING;
    } else if (object->type_id == ARRAY) {
        if (index > _get_len(object->value) - 1 || !_get_len(object->value)) throw_arithmetical_exception(expression_as_string, VIARRAY_RANGE_EXCEPTION);
        Array * element = ((Array **)(object->value))[index];
        result->value = copy_data(element->element, element->type_id);
        result->type_id = element->type_id;
    }
    return result;
}

Constant * _get_range(Constant * object, int start, int end) {
    if (start > end) throw_arithmetical_exception(expression_as_string, VIARRAY_INDEX_EXCEPTION);
    Constant * result = malloc(sizeof(Constant));
    if (object->type_id == STRING) {
        if (end > strlen(object->value) - 1) throw_arithmetical_exception(expression_as_string, VIARRAY_RANGE_EXCEPTION);
        char substring[ARITHMETICA_MAX_STRING_LEN] = "\0"; memcpy(substring, object->value, sizeof(char) * (end - start + 1));
        result->value = alloc_string(substring);
        result->type_id = STRING;
    } else if (object->type_id == ARRAY) {
        if (end > _get_len(object->value) - 1) throw_arithmetical_exception(expression_as_string, VIARRAY_RANGE_EXCEPTION);
        Array ** mainarray = object->value;
        Array * tmp_subarray[end - start + 1]; tmp_subarray[end - start + 1] = 0; // setting end of array
        memcpy(tmp_subarray, mainarray + start, sizeof(Array *) * (end - start + 1));
        Array ** subarray = copy_array(subarray, tmp_subarray);
        result->value = subarray;
        result->type_id = ARRAY;
    }
    return result;
}

Constant * _get_range_step(Constant * object, int start, int end, int step) {
    if (start > end || (start + step) > end) throw_arithmetical_exception(expression_as_string, VIARRAY_INDEX_EXCEPTION);
    Constant * result = malloc(sizeof(Constant));
    if (object->type_id == STRING) {
        if (end > strlen(object->value) - 1) throw_arithmetical_exception(expression_as_string, VIARRAY_RANGE_EXCEPTION);
        char substring[ARITHMETICA_MAX_STRING_LEN] = "\0"; char * object_str = object->value;
        int counter = 0;
        for (int i = start; i <= end; i += step) {
            if (i > strlen(object_str) - 1) throw_arithmetical_exception(expression_as_string, VIARRAY_RANGE_EXCEPTION);
            substring[counter++] = object_str[i];
        }
        result->value = alloc_string(substring);;
        result->type_id = STRING;
    } else if (object->type_id == ARRAY) {
        if (end > _get_len(object->value) - 1) throw_arithmetical_exception(expression_as_string, VIARRAY_RANGE_EXCEPTION);
        Array ** sub_array = new_array(); Array ** object_array = object->value;
        for (int i = start; i <= end; i += step) {
            if (i > _get_len(object_array) - 1) throw_arithmetical_exception(expression_as_string, VIARRAY_RANGE_EXCEPTION);
            sub_array = append(sub_array, object_array[i]->type_id, copy_data(object_array[i]->element, object_array[i]->type_id));
        }
        result->value = sub_array;
        result->type_id = ARRAY;
    }
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