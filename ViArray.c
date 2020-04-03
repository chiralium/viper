#include "ViArray.h"
#include "exception.h"
#include "types.h"
#include "fread.h"
#include "display.h"

extern char * expression_as_string;

// TODO: b => a => VIARRAY; a = 3.14; b = (?). RIGHT: b => VIARRAY, a => 3.14

Node * new_viarray(Array ** array) {
    Node * root = NULL;
    int index = 0;
    while (array[index]) {
        if (!array[index]) break;
        else {
            Constant * array_element = array[index]->element;
            Constant * viarray_element; Node * node;
            if (is_simple_data(array_element->type_id)) {
                viarray_element = new_constant(array_element->type_id, array_element->value);
                node = new_node(index, viarray_element);
                (root == NULL) ? root = insert_node(root, node) : insert_node(root, node);
            } else if (array_element->type_id == ARRAY) {
                node = array_element->value;
                viarray_element = new_constant(VIARRAY, node);
                (root == NULL) ? root = insert_node(root, new_node(index, viarray_element)) : insert_node(root, new_node(index, viarray_element));
            } else if (array_element->type_id == VIARRAY) {
                node = array_element->value;
                viarray_element = new_constant(VIARRAY, node);
                Node * pointer_node = new_node(index, viarray_element); viarray_element->origin = array_element->origin;
                (root == NULL) ? root = insert_node(root, pointer_node) : insert_node(root, pointer_node);
            }
            free(array_element);
        }
        free(array[index]);
        index++;
    }
    free(array);
    return root;
}

Constant * get_by_index(Constant * object, Array ** params) {
    validate_index_parameter(params);
    int params_count = _get_len(params); Constant * result;
    switch (params_count) {
        case 1:
            result = get_single(object, *(int *)(params[0]->element));
            break;
    }
    free(object);
    return result;
}

Constant * get_single(Constant * object, int index) {
    Constant * element;
    if (object->type_id == VIARRAY) {
        Node * viarray = object->value;
        Node * element_node = find_node(viarray, index);

        Constant * array_element;
        if (!element_node) throw_arithmetical_exception(expression_as_string, VIARRAY_RANGE_EXCEPTION);

        array_element = element_node->value;
        element = new_constant(array_element->type_id, array_element->value);
        element->origin = element_node;
    }
    return element;
}

void validate_index_parameter(Array ** params) {
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

void display_viarray(Node * root) {
    if (root != NULL) {
        Constant * array_element = root->value;
        if (array_element->type_id == VIARRAY) {
            printf("["); display_viarray(array_element->value); printf("], ");
        }
        else {
            display_constant(array_element);
            printf(", ");
        }
        display_viarray(root->right);
    }
}