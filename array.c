#include "array.h"
#include "lexer.h"
#include "parser.h"
#include "expression.h"

Array ** new_array(void) {
    Array **_array = (Array **)calloc(2, sizeof(Array *));
    return _array;
}

Array ** copy_array(Array **Y, Array **X) {
    unsigned int l = _get_len(X);
    Y = (Array **)calloc(l + 1, sizeof(Array *));
    int i = 0;
    while (X[i]) {
        Array *el; el = (Array *)malloc(sizeof(Array));
        el -> type_id = X[i] -> type_id;
        if (X[i] -> type_id == INTEGER) {
            int *value; value = (int *)malloc(sizeof(int));
            *value = *(int *)(X[i] -> element);
            el -> element = value;
        } else if (X[i] -> type_id == FLOAT) {
            float *value; value = (float *)malloc(sizeof(float));
            *value = *(float *)(X[i] -> element);
            el -> element = value;
        } else if (X[i] -> type_id == STRING) {
            char *value; value = calloc(strlen((char *)X[i] -> element) + 1, sizeof(char));
            strcpy(value, (char *)X[i] -> element);
            el -> element = value;
        } else if (X[i] -> type_id == ARRAY) {
            Array **sub_array;
            sub_array = copy_array(sub_array, X[i] -> element);
            el -> element = sub_array;
        }
        Y[i] = el;
        i++;
    }
    return Y;
}

Array ** realloc_array(Array **_array, unsigned long nsize) {
    Array **reallocated_array = (Array**)calloc(nsize, sizeof(Array *));
    int array_count, reallocated_array_count; array_count = reallocated_array_count = 0;
    while (_array[array_count]) {
        Array *el; el = malloc(sizeof(Array));
        el -> type_id = _array[array_count] -> type_id;
        el -> element = _array[array_count] -> element;
        reallocated_array[reallocated_array_count] = el;
        free(_array[array_count]);
        array_count++; reallocated_array_count++;
    }
    free(_array);
    return reallocated_array;
}

Array ** append(Array **_array, char type_id, void *value) {
    unsigned long len = _get_len(_array);
    if (len) _array = realloc_array(_array, len + 2);
    Array *el; el = malloc(sizeof(Array));
    el -> type_id = type_id; el -> element = value;
    _array[len] = el;
    return _array;
}

unsigned long _get_len(Array **_array) {
    unsigned long l = 0;
    while (_array[l]) l++;
    return l;
}

Array * pop_el(Array **X) {
    if (*X) {
        int length = _get_len(X);
        Array *first = *X; X++;
        memcpy(X - 1, X, sizeof(Array *) * length);
        X[length - 1] = 0;
        return first;
    } else {
        return 0;
    }
}

Array * get_first_el(Array **X) {
    return *X;
}

int is_empty(Array **X) {
    return !(*X);
}

void array_destructor(Array **_array) {
    int i = 0;
    while (_array[i]) {
        if (_array[i] -> type_id == ARRAY) array_destructor(_array[i] -> element);
        else if (_array[i] -> type_id == TOKEN) token_destructor(_array[i] -> element);
        else if (_array[i] -> type_id == EXP_TK) exp_token_destructor(_array[i]->element);
        else free(_array[i] -> element);
        free(_array[i]);
        i++;
    }
    free(_array);
}
