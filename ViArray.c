#include "ViArray.h"
#include "exception.h"
#include "fread.h"
#include "display.h"
#include "memory.h"

extern char * expression_as_string;
extern Array ** memory_table;

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
            } else if (array_element->type_id == KEYPAIR) {
                Node * keypair = array_element->value;
                (root == NULL) ? root = insert_node(root, keypair) : insert_node(root, keypair);
            } else if (array_element->type_id == VIARRAY) {
                node = array_element->value;
                viarray_element = new_constant(array_element->type_id, node);
                Node * pointer_node = new_node(index, viarray_element);
                (root == NULL) ? root = insert_node(root, pointer_node) : insert_node(root, pointer_node);
            } else if (array_element->type_id == FUNCTION_CONTAINER) {
                viarray_element = new_constant(array_element->type_id, array_element->value);
                Node * pointer_node = new_node(index, viarray_element);
                (root == NULL) ? root = insert_node(root, pointer_node) : insert_node(root, pointer_node);
            }
            free(array_element);
        }
        free(array[index]);
        index++;
    }
    free(array);
    /* allocate create the address into memory_table for allocated memory */
    memory_table = append(memory_table, MEMORY_ELEMENT, new_memory_element(VIARRAY, root, "ViArray.c/new_viarray"));
    return root;
}

Constant * get_by_index(Constant * object, Array ** params) {
    int params_count = _get_len(params);
    Constant * result;
    switch (params_count) {
        case 1:
            result = get_single(object, params[0]->element);
            break;
        case 2:
            validate_index_parameter(params);
            result = get_subviarray(object, params[0]->element, params[1]->element);
            break;
        case 3:
            validate_index_parameter(params);
            result = get_subviarray_step(object, params[0]->element, params[1]->element, params[2]->element);
            break;
        case 0:
            result = get_new(object);
            break;
    }
    free(object);
    return result;
}

Constant * get_subviarray_step(Constant * object, Constant * start, Constant * end, Constant * step) {
    Constant * sub_element;
    int start_indx = *(int *)(start->value); int end_indx = *(int *)(end->value); int step_val = *(int *)(step->value);
    Node * viarray = object->value; Node * subviarray = NULL;
    Node * start_node = find_node(viarray, start_indx); Node * end_node = find_node(viarray, end_indx);
    if (start_node == NULL || end_node == NULL) throw_arithmetical_exception(expression_as_string, VIARRAY_RANGE_EXCEPTION);
    Node * current_node = start_node; int index = 0; Constant * copied_element;
    while (start_indx <= end_indx || current_node != NULL) {
        Constant * element = current_node->value;

        copied_element = new_constant(element->type_id, copy_data(element->value, element->type_id));
        if (!is_simple_data(element->type_id)) copied_element->origin = element->value; // is the element is a complex data that stored into namespace set the origin

        Node * subviarray_node = new_node(index++, copied_element);
        (subviarray == NULL) ? subviarray = insert_node(subviarray, subviarray_node) : insert_node(subviarray, subviarray_node);
        start_indx += step_val; current_node = find_node(viarray, start_indx);
    }
    sub_element = new_constant(VIARRAY, subviarray);
    memory_table = append(memory_table, MEMORY_ELEMENT, new_memory_element(VIARRAY, subviarray, "ViArray.c/get_subviarray_step"));
    return sub_element;
}

Constant * get_subviarray(Constant * object, Constant * start, Constant * end) {
    Constant * sub_element;
    int start_indx = *(int *)(start->value); int end_indx = *(int *)(end->value);
    Node * viarray = object->value; Node * subviarray = NULL;
    Node * start_node = find_node(viarray, start_indx); Node * end_node = find_node(viarray, end_indx);
    if (start_node == NULL || end_node == NULL) throw_arithmetical_exception(expression_as_string, VIARRAY_RANGE_EXCEPTION);

    Node * current_node = start_node; int index = 0; Constant * copied_element;
    while (current_node->key <= end_indx) {
        Constant * element = current_node->value;

        if (element->type_id == VIARRAY) copied_element = new_constant(VIARRAY, copy_viarray(element->value));
        else copied_element = new_constant(element->type_id, copy_data(element->value, element->type_id));

        Node * subviarray_node = new_node(index++, copied_element);
        (subviarray == NULL) ? subviarray = insert_node(subviarray, subviarray_node) : insert_node(subviarray, subviarray_node);
        if (current_node->right != NULL) current_node = current_node->right;
        else throw_internal_error(expression_as_string);
    }
    sub_element = new_constant(VIARRAY, subviarray);
    memory_table = append(memory_table, MEMORY_ELEMENT, new_memory_element(VIARRAY, subviarray, "ViArray.c/get_subviarray"));
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
    Constant * element;
    if (object->type_id == VIARRAY) {
        int index_value;
        if (index->type_id == INTEGER) index_value = *(int *)(index->value);
        else if (index->type_id == FLOAT) {
            char float_to_string[EXPRESSION_MAX_LEN]; gcvt( *(float *)(index->value), 10, float_to_string);
            index_value = faq6(float_to_string);
        } else if (index->type_id == STRING) index_value = faq6(index->value);
        else throw_arithmetical_exception(expression_as_string, VIARRAY_INVALID_KEY_TYPE);

        Node *viarray = object->value;
        Node *element_node = find_node(viarray, index_value);

        Constant *array_element;
        if (!element_node) throw_arithmetical_exception(expression_as_string, VIARRAY_RANGE_EXCEPTION);

        array_element = element_node->value;
        element = new_constant(array_element->type_id, array_element->value);
        element->origin = element_node;
    } else if (object->type_id == KEYPAIR) {
        int index_value;
        if (index->type_id == INTEGER) index_value = *(int *)(index->value);
        else if (index->type_id == FLOAT) {
            char float_to_string[EXPRESSION_MAX_LEN]; gcvt( *(float *)(index->value), 10, float_to_string);
            index_value = faq6(float_to_string);
        } else if (index->type_id == STRING) index_value = faq6(index->value);
        else throw_arithmetical_exception(expression_as_string, VIARRAY_INVALID_KEY_TYPE);

        Node * keypair_array = object->value;
        Node * element_node = find_node(keypair_array, index_value);

        Constant * array_element;
        if (!element_node) throw_arithmetical_exception(expression_as_string, VIARRAY_UNDEFINED_KEY);
        array_element = element_node->value;
        element = new_constant(array_element->type_id, array_element->value);
        element->origin = element_node;
    }
    return element;
}

Node * copy_viarray(Node * viarray) {
    Node * copied_viarray = NULL;
    while (viarray != NULL) {
        Constant * element = viarray->value; Constant * copied_value = NULL;

        if (is_simple_data(element->type_id)) copied_value = new_constant(element->type_id, copy_data(element->value, element->type_id));
        else if (element->type_id == VIARRAY) copied_value = new_constant(VIARRAY, copy_viarray(element->value));
        else throw_internal_error(expression_as_string);

        Node * copied_node = new_node(viarray->key, copied_value);
        (copied_viarray == NULL) ? copied_viarray = insert_node(copied_viarray, copied_node) : insert_node(copied_viarray, copied_node);
        viarray = viarray->right;
    }
    (copied_viarray != NULL) ? memory_table = append(memory_table, MEMORY_ELEMENT, new_memory_element(VIARRAY, copied_viarray, "ViArray.c/copy_viarray")) : NULL;
    return copied_viarray;
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
            if (root->raw_key == NULL) printf("[%d] => ", root->key);
            else printf("[`%s`] => ", root->raw_key);
            printf("{");
            display_viarray(array_element->value);
            printf("}, ");
        }
        else {
            if (root->raw_key == NULL) printf("[%d] => ", root->key);
            else printf("[`%s`] => ", root->raw_key);
            display_constant(array_element);
            printf(", ");
        }
        display_viarray(root->right);
        display_viarray(root->left);
    }
}