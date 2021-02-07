#include "array.h"
#include "stdio.h"
#include "types.h"
#include "display.h"
#include "test.h"

void test() {
    printf("Testing mode!\n");

    int test_array_result = test_array();

    printf("Status: %s\n", test_array_result ? "PASSED" : "FAILED");
}

/* Arrays */
int test_array() {
    printf("\nTest #0: array.h:\n\n");
    return test_get_last_el() &&
           test_append() &&
           test_insert() &&
           test_get_len() &&
           test_pop_el() &&
           test_pop_last_el()
           ;
}

int test_get_last_el() {
    printf("get_last_el: ");
    Array ** test_array = new_array();
    int * test_value = malloc(sizeof(int));
    *test_value = 1;

    test_array = append(test_array, INTEGER, test_value);
    int * expect_value = ((Array *)get_last_el(test_array))->element;
    char expect_type = ((Array *)get_last_el(test_array))->type_id;

    printf(
            "%s\n",
            *expect_value == *test_value && expect_type == INTEGER ? "OK" : "FAIL"
    );

    return *expect_value == *test_value && expect_type == INTEGER;
}

int test_append() {
    printf("append: ");
    int *i = malloc(sizeof(int)); *i = 2;
    Array * first = malloc(sizeof(Array *));
    first->type_id = INTEGER;
    first->element = i;


    Array * expect_array[] = {first, 0};

    Array ** test_array = new_array();
    test_array = append(test_array, INTEGER, i);

    printf(
            "%s\n",
        (*test_array)->type_id == expect_array[0]->type_id &&
        (int *)((*test_array)->element) == (int *)(expect_array[0]->element) ? "OK" : "FAIL"
    );

    return (*test_array)->type_id == expect_array[0]->type_id &&
           (int *)((*test_array)->element) == (int *)(expect_array[0]->element);
}


int test_insert() {
    printf("insert: ");
    int * a = malloc(sizeof(int));
    int * b = malloc(sizeof(int));
    int * c = malloc(sizeof(int));
    *a = 1; *b = 2; *c = 3;

    Array *ar_a = malloc(sizeof(Array *));
    Array *ar_b = malloc(sizeof(Array *));
    Array *ar_c = malloc(sizeof(Array *));

    ar_a->type_id = INTEGER; ar_b->type_id = INTEGER; ar_c->type_id = INTEGER;
    ar_a->element = a; ar_b->element = b; ar_c->element = c;

    Array * expect_array[] = { ar_a, ar_c, ar_b, 0};

    Array ** test_array = new_array();
    test_array = append(test_array, INTEGER, a);
    test_array = append(test_array, INTEGER, b);
    test_array = insert(test_array, INTEGER, c, 1);

    int result = array_cmp(expect_array, test_array);
    printf("%s\n", result ? "OK" : "FAILED");

    return result;
}

int test_get_len() {
    printf("_get_len: ");
    int * a = malloc(sizeof(int));
    int * b = malloc(sizeof(int));
    int * c = malloc(sizeof(int));
    *a = 1; *b = 2; *c = 3;

    Array *ar_a = malloc(sizeof(Array *));
    Array *ar_b = malloc(sizeof(Array *));
    Array *ar_c = malloc(sizeof(Array *));

    ar_a->type_id = INTEGER; ar_b->type_id = INTEGER; ar_c->type_id = INTEGER;
    ar_a->element = a; ar_b->element = b; ar_c->element = c;

    Array * expect_array[] = { ar_a, ar_c, ar_b, 0 };

    unsigned long expect_len = 3;

    int result = _get_len(expect_array) == expect_len;

    printf("%s\n", result ? "OK" : "FAILED");
    return result;
}

int test_pop_last_el() {
    printf("pop_last_el: ");
    int * a = malloc(sizeof(int));
    int * b = malloc(sizeof(int));
    int * c = malloc(sizeof(int));
    *a = 1; *b = 2; *c = 3;

    Array *ar_a = malloc(sizeof(Array *));
    Array *ar_b = malloc(sizeof(Array *));
    Array *ar_c = malloc(sizeof(Array *));

    ar_a->type_id = INTEGER; ar_b->type_id = INTEGER; ar_c->type_id = INTEGER;
    ar_a->element = a; ar_b->element = b; ar_c->element = c;

    Array * expect_array[] = { ar_a, ar_c, ar_b, 0 };

    Array * testing_el = pop_last_el(expect_array);

    int result = ( testing_el->type_id == ar_b->type_id ) && ( (int *)(ar_b->element) == (int *)(testing_el->element) );
    printf("%s\n", result ? "OK" : "FAILED");
    return result;
}


int test_pop_el() {
    printf("pop_el: ");
    int * a = malloc(sizeof(int));
    int * b = malloc(sizeof(int));
    int * c = malloc(sizeof(int));
    *a = 1; *b = 2; *c = 3;

    Array *ar_a = malloc(sizeof(Array *));
    Array *ar_b = malloc(sizeof(Array *));
    Array *ar_c = malloc(sizeof(Array *));

    ar_a->type_id = INTEGER; ar_b->type_id = INTEGER; ar_c->type_id = INTEGER;
    ar_a->element = a; ar_b->element = b; ar_c->element = c;

    Array * expect_array[] = { ar_a, ar_c, ar_b, 0 };

    Array * testing_el = pop_el(expect_array);

    int result = ( testing_el->type_id == ar_a->type_id ) && ( (int *)(ar_a->element) == (int *)(testing_el->element) );
    printf("%s\n", result ? "OK" : "FAILED");
    return result;
}

/* Utils */
int array_cmp(Array ** a, Array ** b) {
    unsigned long a_length = _get_len(a);
    unsigned long b_length = _get_len(b);

    if ( a_length != b_length ) return 0;
    else {
        int counter = 0;
        while( a[counter] ) {
            if ( a[counter]->type_id != b[counter]->type_id ) return 0;
            else {
                if ( (int *)(a[counter]->element) != (int *)(b[counter]->element) ) return 0;
            }
            counter++;
        }
    }
    return 1;
}

/* Arrays */