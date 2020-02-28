#include "display.h"

void display_array(Array ** _array) {
    printf("[");
    int array_counter = 0;
    while (_array[array_counter]) {
        if (_array[array_counter] -> type_id == INTEGER) printf("`%d`, ", *(int *)_array[array_counter] -> element);
        else if (_array[array_counter] -> type_id == FLOAT) printf("`%d`, ", *(float *)_array[array_counter] -> element);
        else if (_array[array_counter] -> type_id == STRING) printf("`%s`, ", (char *)_array[array_counter] -> element);
        else if (_array[array_counter] -> type_id == ARRAY) {
            display_array((Array **)(_array[array_counter] -> element)); printf(", ");
        }
        array_counter++;
    }
    printf("]<a>(0x%p)", _array);
}