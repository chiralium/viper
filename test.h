/*
 * Testing
 */


#ifndef VIPER_V4_TEST_H
#define VIPER_V4_TEST_H

/* Main entrypoint */

void test(void);

/* Test #1: array.h */
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

/* Utils */
int array_cmp(Array ** a, Array ** b);

#endif
