#include "display.h"

void display_array(Array ** _array) {
    printf("[");
    int array_counter = 0;
    while (_array[array_counter]) {
        if (_array[array_counter] -> type_id == INTEGER) printf("`%d`, ", *(int *)_array[array_counter] -> element);
        else if (_array[array_counter] -> type_id == FLOAT) printf("`%d`, ", *(float *)_array[array_counter] -> element);
        else if (_array[array_counter] -> type_id == STRING) printf("`%s`, ", (char *)_array[array_counter] -> element);
        else if (_array[array_counter] -> type_id == TOKEN) printf ("{TK:%d; `%s`}, ", ((Token *)(_array[array_counter] -> element)) -> type_id,
                                                                                       (char *)((Token *)(_array[array_counter] -> element)) -> value);
        else if (_array[array_counter] -> type_id == NULL_TOKEN) printf("{TK:NULL}, ");
        else if (_array[array_counter] -> type_id == STMT_IF) printf("{STMT_IF; `%s`}, ", ((If *)(_array[array_counter] -> element)) -> condition);
        else if (_array[array_counter] -> type_id == COMPLEX_TOKEN) printf("{TK:%d; `<complex>`}, ", ((Token *)(_array[array_counter] -> element)) -> type_id);
        else if (_array[array_counter] -> type_id == ARRAY) {
            display_array((Array **)(_array[array_counter] -> element)); printf(", ");
        }
        array_counter++;
    }
    printf("]<a>(0x%p)", _array);
}

void display_statements(void * statement, char type_id) {
    If * if_statement;
    switch (type_id) {
        case 1:
            /* if-else statement */
            if_statement = statement;
            printf("\nif `%s` ", if_statement->condition); display_array(if_statement->body);
            if (if_statement->else_body) {
                printf(" [else] `%s` ", if_statement->else_condition ? if_statement->else_condition : "<empty>");
                display_array(if_statement->else_body);
            }
            break;
    }
}