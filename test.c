#include "array.h"
#include "fread.h"

#include "stdio.h"
#include "types.h"
#include "display.h"
#include "test.h"

void test() {
    printf("Testing mode!\n");

    int test_array_result = test_array();
    printf("\nStatus: %s\n", test_array_result ? "PASSED" : "FAILED");

    int test_fread_result = test_fread();
    printf("\nStatus: %s\n", test_fread_result ? "PASSED" : "FAILED");
}

/* Fread */
int test_fread() {
    printf("\nTest #1: fread.h\n");
    return test_cut_node_structure() &&
           test_cut_complex_structure() &&
           test_recursive_descent()
           ;
}

int test_recursive_descent() {
    int result = 1;
    printf("\ntest_recursive_descent:\n");
    char * testing_data[] = {
            "2+2",
            "2 * 3 / 4 + (5 / 3.14)",
            "if ( a > 0 ) : {...}",
            "while ( a > 0 ) : { a += 1; b = 1 }",
            "if ( a > 0 ) : { a += 1; if ( a > 3 ) : {...} }",
            0,
    };

    Array ** expect_data = new_array();

    Array ** by_line = new_array();
    by_line = append(by_line, STRING, alloc_string("2+2"));
    expect_data = append( expect_data, ARRAY, by_line );

    by_line = new_array();
    by_line = append(by_line, STRING, alloc_string("2 * 3 / 4 + (5 / 3.14)"));
    expect_data = append( expect_data, ARRAY, by_line );

    by_line = new_array();
    by_line = append(by_line, STRING, alloc_string("if ( a > 0 ) "));
    Array ** temp = new_array(); temp = append(temp, STRING, alloc_string("..."));
    by_line = append(by_line, ARRAY, temp);
    expect_data = append( expect_data, ARRAY, by_line );

    by_line = new_array();
    by_line = append(by_line, STRING, alloc_string("while ( a > 0 ) "));

    temp = new_array();
    temp = append(temp, STRING, alloc_string("a += 1"));
    temp = append(temp, STRING, alloc_string("b = 1 "));
    by_line = append(by_line, ARRAY, temp);
    expect_data = append( expect_data, ARRAY, by_line );

    by_line = new_array();
    by_line = append(by_line, STRING, alloc_string("if ( a > 0 ) "));

    temp = new_array();
    temp = append(temp, STRING, alloc_string("a += 1"));
    temp = append(temp, STRING, alloc_string("if ( a > 3 ) "));
    Array ** temp2 = new_array();
    temp2 = append(temp2, STRING, alloc_string("..."));
    temp = append(temp, ARRAY, temp2);

    by_line = append(by_line, ARRAY, temp);
    expect_data = append( expect_data, ARRAY, by_line );

    int counter = 0;
    while ( testing_data[counter] ) {
        char *input_stream = alloc_string(testing_data[counter]);
        Array ** result_data = recursive_descent(input_stream);
        result *= array_cmp( expect_data[counter]->element, result_data);

        printf("input: `%s` -- %s => ", testing_data[counter], result ? "OK" : "FAIL");
        display_array(result_data);
        printf("\n");


        counter++;
    }


    return result;
}

int test_cut_complex_structure() {
    int result = 1;
    printf("\ntest_cut_complex_structure:\n");

    char * testing_data[] = {
            "{...}",
            "{1, 2, 3}",
            "{1, 2, {3, 4}, 5}",
            0,
    };

    char * expect_data[] = {
            "...",
            "1, 2, 3",
            "1, 2, {3, 4}, 5",
            0,
    };

    int counter = 0;
    while ( testing_data[counter] ) {
        char *input_stream = alloc_string(testing_data[counter]);
        char *result_string = cut_complex_structure(input_stream);
        char *expect = expect_data[counter];

        result *= !strcmp(expect, result_string);
        printf("input: `%s` -- %s => / %s /\n", testing_data[counter], result ? "OK" : "FAIL", result_string);
        counter++;
    }

    return result;
}

int test_cut_node_structure() {
    int result = 1;
    printf("\ntest_cut_node_structure:\n");
    char * testing_data[] = {
            "2;",
            "2",
            "2 + 2",
            "2\n+2",
            "2; 3+4",
            "\"hello world\"",
            "if (a > 2) : {...}",
            "{1, 2, 3}",
            "{1, {2, 3}, 4}",
            "{1, }",
            "(2+2)*3",
            "string = \"hello + world \\n\";",
            0
    };

    char * expect_data[] = {
            "2",
            "2",
            "2 + 2",
            "2+2",
            "2",
            "\"hello world\"",
            "if (a > 2) ",
            "{1, 2, 3}",
            "{1, {2, 3}, 4}",
            "{1, }",
            "(2+2)*3",
            "string = \"hello + world \\n\"",
            0
    };

    int counter = 0;
    while ( testing_data[counter] ) {
        char *input_stream = alloc_string(testing_data[counter]);
        char *result_string = cut_structure_node(input_stream);
        char *expect = expect_data[counter];

        result *= !strcmp(expect, result_string);
        printf("input: `%s` -- %s => / %s /\n", testing_data[counter], result ? "OK" : "FAIL", result_string);
        counter++;
    }

    return result;
}

/* Fread */

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
                switch (a[counter]->type_id) {
                    case INTEGER:
                        if ( (int *)(a[counter]->element) != (int *)(b[counter]->element) ) return 0;
                        break;
                    case STRING:
                        if ( strcmp(a[counter]->element, b[counter]->element) ) return 0;
                        break;
                    case ARRAY:
                        return array_cmp(a[counter]->element, b[counter]->element);
                        break;
                }

            }
            counter++;
        }
    }
    return 1;
}

/* Arrays */