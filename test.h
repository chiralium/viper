/*
 * Testing
 */


#ifndef VIPER_V4_TEST_H
#define VIPER_V4_TEST_H

/* Main entrypoint */

void test(void);

/* Test #0: array.h */
int test_array();
    /* Testing the get_last_el */
    int test_get_last_el();

    /* Testing the append */
    int test_append();

    /* Testing the insert */
    int test_insert();

    /* Testing the _get_len */
    int test_get_len();

    /* Testing pop_el */
    int test_pop_el();

    /* Testing pop_last_el */
    int test_pop_last_el();

/* Test #1: fread.h */
int test_fread();
    /* Testing cut_structure_node */
    int test_cut_node_structure();

    /* Testing cut_complex_structure */
    int test_cut_complex_structure();

    /* Testing recursive_descent */
    int test_recursive_descent();

/* Test #2: lexer.h */
int test_lexer();
    /* Testing cut_token */
    int test_cut_token();

    /* Testing is_keyword */
    int test_is_keyword();

    /* Testing extract_token */
    int test_extract_token();

    /* Testing lexer */
    int _test_lexer();

/* Test #3: viper */
int test_viper();
    /* Testing assign feature (integer) */
    int test_assign_int();
    /* Testing assign feature (string) */
    int test_assign_string();

/* Utils */
int array_cmp(Array ** a, Array ** b);
Constant * wrapper(char * input_stream);

#endif
