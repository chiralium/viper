#include "ViArray.h"
#include "exception.h"
#include "types.h"
#include "fread.h"
#include "display.h"

extern char * expression_as_string;

Node * new_viarray(Array ** array) {
    Node * root = (_get_len(array) == 0) ? new_node(0, new_constant(UNDEFINED, NULL)) : NULL;
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
            } else if (array_element->type_id == VIARRAY) {
                node = array_element->value;
                viarray_element = new_constant(VIARRAY, node);
                Node * pointer_node = new_node(index, viarray_element);
                (root == NULL) ? root = insert_node(root, pointer_node) : insert_node(root, pointer_node);
                if (array_element->origin != NULL) {
                    Constant * complex_value = ( (Node *)(array_element->origin) )->value;
                    complex_value->origin = pointer_node;
                }
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
    int params_count = _get_len(params);
    Constant * result;
    switch (params_count) {
        case 1:
            result = get_single(object, params[0]->element);
            break;
        case 2:
            result = get_subviarray(object, params[0]->element, params[1]->element);
            break;
        case 0:
            result = get_new(object);
            break;
    }
    free(object);
    return result;
}

// TODO: a = {1, 2, {3, 4}, 5}; a[0,3] => namespace destructor error

Constant * get_subviarray(Constant * object, Constant * start, Constant * end) {
    Constant * sub_element; int start_indx = *(int *)(start->value); int end_indx = *(int *)(end->value);
    if (object->type_id == VIARRAY) {
        Node * viarray = object->value; Node * subviarray = NULL;
        Node * start_node = find_node(viarray, start_indx); Node * end_node = find_node(viarray, end_indx);
        if (start_node == NULL || end_node == NULL) throw_arithmetical_exception(expression_as_string, VIARRAY_RANGE_EXCEPTION);

        Node * current_node = start_node; int index = 0;
        while (current_node->key <= end_indx) {
            Constant * element = current_node->value;
            Constant * copied_element = new_constant( element->type_id, copy_data(element->value, element->type_id) ); copied_element->origin = element->origin;
            Node * subviarray_node = new_node(index++, copied_element);
            (subviarray == NULL) ? subviarray = insert_node(subviarray, subviarray_node) : insert_node(subviarray, subviarray_node);
            if (current_node->right != NULL) current_node = current_node->right;
            else throw_internal_error(expression_as_string);
        }
        sub_element = new_constant(VIARRAY, subviarray);
    }
    return sub_element;
}

int get_length(Node * viarray) {
    if (viarray == NULL) return 0;
    else return 1 + get_length(viarray->right);
}

Constant * get_new(Constant * object) {
    Constant * element;
    if (object->type_id == VIARRAY) {
        Node * viarray = object->value; Constant * first_element = viarray->value;
        if ( first_element->type_id != UNDEFINED ) {
            // if viarray is not an empty, create new node with constant structure with undefined type, insert at last position and return it
            int last_position = get_length(viarray);
            Constant * new_value_of_element = new_constant(UNDEFINED, NULL);
            Node * new_viarray_element = new_node(last_position, new_value_of_element);
            insert_node(viarray, new_viarray_element);
            element = new_constant(UNDEFINED, new_value_of_element);
            element->origin = new_viarray_element;
        } else {
            // if viarray is an empty, that means the first Constant-structure has undefined type. Just return it
            element = new_constant(UNDEFINED, viarray);
            element->origin = viarray;
        }
    } else throw_arithmetical_exception(expression_as_string, VIARRAY_NEW_ELEMENT_ERR);
    return element;
}

Constant * get_single(Constant * object, Constant * index) {
    Constant * element; int index_value = *(int *)(index->value);
    if (object->type_id == VIARRAY) {
        Node * viarray = object->value;
        Node * element_node = find_node(viarray, index_value);

        Constant * array_element;
        if (!element_node) throw_arithmetical_exception(expression_as_string, VIARRAY_RANGE_EXCEPTION);

        array_element = element_node->value;
        element = new_constant(array_element->type_id, array_element->value);
        element->origin = element_node;
    } else if (object->type_id == STRING) {
        char * string = object->value;
        if (strlen(string) <= index_value) throw_arithmetical_exception(expression_as_string, VIARRAY_RANGE_EXCEPTION);
        char stack_tmp[2] = "\0"; stack_tmp[0] = string[index_value];
        element = new_constant(STRING, alloc_string(stack_tmp));
        if (object->origin == NULL) free(string);
    }
    return element;
}

void validate_index_parameter(Array ** params) {
    while (*params) {
        Constant * parameter = (*params)->element;
        switch (parameter->type_id) {
            case INTEGER:
                if (*((int *)parameter->value) < 0) throw_arithmetical_exception(expression_as_string, VIARRAY_INDEX_EXCEPTION);
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