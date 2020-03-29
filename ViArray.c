#include "ViArray.h"
#include "exception.h"

Constant * get_by_index(Constant * object, Array ** params) {
    int params_count = _get_len(params);
    Constant * result = malloc(sizeof(Constant));

    switch(params_count) {
        case 1:
            if (object->type_id == STRING) {
                char * single_symbol = malloc(sizeof(char) * 2);
                single_symbol[0] = ((char *)(object->value))[*(int *)(params[0]->element)];
                single_symbol[1] = '\0';
                result->value = single_symbol;
                result->type_id = STRING;
            } else if (object->type_id == ARRAY) {
                Array * element = ((Array **)(object->value))[*(int *)(params[0]->element)];
                result->value = copy_data(element->element, element->type_id);
                result->type_id = element->type_id;
            }
    }
    constant_destructor(object);
    return result;
}