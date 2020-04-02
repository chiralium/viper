#include "ViArray.h"
#include "exception.h"
#include "types.h"
#include "fread.h"
#include "display.h"

extern char * expression_as_string;

Node * new_viarray(Array ** array) {
    Node * root = NULL;
    int index = 0;
    while (array[index]) {
        if (!array[index]) break;
        else {
            Constant * array_element; Node * node;
            switch (array[index]->type_id) {
                case INTEGER:
                case FLOAT:
                case STRING:
                    array_element = new_constant(array[index]->type_id, array[index]->element);
                    node = new_node(index, array_element);
                    (root == NULL) ? root = insert_node(root, node) : insert_node(root, node);
                    break;
                default:
                    node = array[index]->element;
                    array_element = new_constant(VIARRAY, node);
                    (root == NULL) ? root = insert_node(root, new_node(index, array_element)) : insert_node(root, new_node(index, array_element));
            }
        }
        free(array[index]);
        index++;
    }
    free(array);
    return root;
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