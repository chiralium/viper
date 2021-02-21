#include "array.h"
#include "fread.h"
#include "lexer.h"
#include "interpreter.h"
#include "display.h"

#include "stdio.h"
#include "types.h"
#include "display.h"
#include "test.h"

Array ** call_stack;
Array ** memory_table;
Node * global_namespace;

void test() {
    printf("Testing mode!\n");

    int test_array_result = test_array();
    printf("\nStatus: %s\n", test_array_result ? "PASSED" : "FAILED");

    int test_fread_result = test_fread();
    printf("\nStatus: %s\n", test_fread_result ? "PASSED" : "FAILED");

    int test_lexer_result = test_lexer();
    printf("\nStatus: %s", test_lexer_result ? "PASSED" : "FAILED");

    int test_viper_result = test_viper();
    printf("\nStatus %s", test_viper_result ? "PASSED" : "FAILED");
}
/* Viper */
int test_viper() {
    printf("\nTest #3: viper\n");
    return test_assign_int() &&
           test_assign_string()
           ;
}

int test_assign_string() {
    printf("\ntest_assign_string\n");
    int result = 1;
    char testing_data[1024] = "a = \"hello world!!!\"; b = \"hello \" + \"world!\"; c = a + b + string(123);";

    strcat(testing_data,
           "newline = \"function A : {output(1)}\";");
    strcat(testing_data,
           "q = \"\\\"hello world\\\"\";");
    strcat(testing_data,
           "s = \"hello\"[0];");
    strcat(testing_data,
           "d = \"hello\"[0,3];");
    strcat(testing_data,
           "abc = \"abcdefghijklmnopqrst\"[0, 6, 2];");
    strcat(testing_data,
           "concat = string(3.14) + \" is number of \\\"pi\\\"\";");

    printf("input: %s\n", testing_data);
    char * keys[] = {
            "a",
            "b",
            "c",
            "newline",
            "q",
            "s",
            "d",
            "abc",
            "concat",
            0
    };
    char * result_data[] = {
            "hello world!!!",
            "hello world!",
            "hello world!!!hello world!123",
            "function A : {output(1)}",
            "\"hello world\"",
            "h",
            "hel",
            "aceg",
            "3.140000 is number of \"pi\"",
            0,
    };

    int counter = 0;
    char * input_string = alloc_string(testing_data);
    wrapper(input_string);

    while (keys[counter]) {
        Node * expected = find_node(global_namespace, faq6(keys[counter]));
        Constant * value = expected->value;
        result *= value->type_id == STRING && !strcmp((char *)(value->value), result_data[counter]);
        printf("key: %s, [%s] -> [%s]\n", keys[counter], result ? "OK" : "FAIL", (char *)(value->value));
        counter++;
    }
    return result;
}

int test_assign_int() {
    printf("\ntest_assign_int:\n");
    int result = 1;
    char * testing_data = "a = 3; b = 7; c = 1 + 2; d = a + b; e = d / b; formula = 2 + 314; formula = (3 + 314 + formula) + formula; f = integer(\"6996\")";
    char * keys[] = {
            "a",
            "b",
            "c",
            "d",
            "e",
            "formula",
            "f",
            0
    };
    int result_data[] = {
            3,
            7,
            3,
            10,
            9 / 7,
            949,
            6996,
    };

    int counter = 0;
    char * input_string = alloc_string(testing_data);
    wrapper(input_string);

    while (keys[counter]) {
        Node * expected = find_node(global_namespace, faq6(keys[counter]));
        Constant * value = expected->value;
        result *= value->type_id == INTEGER && *(int *)(value->value) == result_data[counter];
        printf("key: %s, [%s] -> [%d]\n", keys[counter], result ? "OK" : "FAIL", *(int *)(value->value));
        counter++;
    }
    return result;
}


/* Lexer */
int test_lexer() {
    printf("\nTest #2: lexer.h\n");
    return test_cut_token() &&
           test_is_keyword() &&
           test_extract_token() &&
           _test_lexer()
           ;
}

int _test_lexer() {
    int result = 1;
    printf("\ntest_lexer:\n");

    Array ** testing_data = new_array();
    Array ** testing_literals = new_array();
    testing_literals = append(testing_literals, STRING, alloc_string("2+2"));
    testing_data = append(testing_data, ARRAY, testing_literals);


    testing_literals = new_array();
    testing_literals = append(testing_literals, STRING, alloc_string("while ( a > 0 ) "));
    Array ** temp = new_array();
    temp = append(temp, STRING, alloc_string("a += 1"));
    temp = append(temp, STRING, alloc_string("b = 1 "));
    testing_literals = append(testing_literals, ARRAY, temp);
    testing_data = append( testing_data, ARRAY, testing_literals );


    Array ** expect_data = new_array(); Array ** by_line = new_array();

    Token * expression_token = (Token *)malloc(sizeof(Token));
    expression_token->type_id = LEXER_EXPRESSION_TK;
    expression_token->value = alloc_string("2+2");
    by_line = append(by_line, TOKEN, expression_token);
    expect_data = append( expect_data, ARRAY, by_line );

    Token * keyword_token = (Token *)malloc(sizeof(Token));
    keyword_token->type_id = LEXER_WHILE_TK;
    keyword_token->value = alloc_string("while");

    Token * param_token = (Token *)malloc(sizeof(Token));
    param_token->type_id = LEXER_KEYWORD_PARAM_TK;
    keyword_token->value = alloc_string(" ( a > 0 ) ");

    Token * complex_token = (Token *)malloc(sizeof(Token));
    complex_token->type_id = LEXER_COMPLEX_TK;
    complex_token->value = temp;
    by_line = new_array();

    by_line = append(by_line, TOKEN, keyword_token);
    by_line = append(by_line, TOKEN, param_token);
    by_line = append(by_line, TOKEN, complex_token);
    expect_data = append( expect_data, ARRAY, by_line);

    int counter = 0;
    while ( testing_data[counter] ) {
        Array ** result_data = new_array();
        printf("input: "); display_array(testing_data[counter]->element);
        result_data = lexer(testing_data[counter]->element);
        result *= array_cmp(result_data, expect_data[counter]->element);

        printf(" -- %s", result ? "OK" : "FAIL");
        printf(" => ");
        display_array(result_data);

        printf("\n");

        counter++;
    }
    return result;
}

int test_extract_token() {
    int result = 1;
    printf("\ntest_extract_token:\n");

    char * testing_data[] = {
            "2+2",
            "-2",
            "+",
            "-2 + (-3.14 * (3 / 0.4))",
            "if ( a > 3 ) : {...}",
            "2+2; if ( a > 3 ) : { a = 3; }",
            0
    };

    Array ** expect_data = new_array(); Array ** by_line = new_array();
    Token * expression_token = (Token *)malloc(sizeof(Token));
    expression_token->type_id = LEXER_EXPRESSION_TK;
    expression_token->value = alloc_string("2+2");
    by_line = append(by_line, TOKEN, expression_token);
    expect_data = append( expect_data, ARRAY, by_line );


    by_line = new_array();
    expression_token = (Token *)malloc(sizeof(Token));
    expression_token->type_id = LEXER_EXPRESSION_TK;
    expression_token->value = alloc_string("-2");
    by_line = append(by_line, TOKEN, expression_token);
    expect_data = append( expect_data, ARRAY, by_line );


    by_line = new_array();
    expression_token = (Token *)malloc(sizeof(Token));
    expression_token->type_id = LEXER_EXPRESSION_TK;
    expression_token->value = alloc_string("+");
    by_line = append(by_line, TOKEN, expression_token);
    expect_data = append( expect_data, ARRAY, by_line );


    by_line = new_array();
    expression_token = (Token *)malloc(sizeof(Token));
    expression_token->type_id = LEXER_EXPRESSION_TK;
    expression_token->value = alloc_string("-2 + (-3.14 * (3 / 0.4))");
    by_line = append(by_line, TOKEN, expression_token);
    expect_data = append( expect_data, ARRAY, by_line );


    by_line = new_array();
    Token * if_token = (Token *)malloc(sizeof(Token));
    if_token->type_id = LEXER_IF_TK;
    if_token->value = alloc_string("if");

    Token * if_token_param = (Token *)malloc(sizeof(Token));
    if_token_param->type_id = LEXER_KEYWORD_PARAM_TK;
    if_token_param->value = alloc_string(" ( a > 3 ) : {...}");

    by_line = append(by_line, TOKEN, if_token);
    by_line = append(by_line, TOKEN, if_token_param);
    expect_data = append( expect_data, ARRAY, by_line );


    by_line = new_array();
    expression_token = (Token *)malloc(sizeof(Token));
    expression_token->type_id = LEXER_EXPRESSION_TK;
    expression_token->value = alloc_string("2+2; if ( a > 3 ) : { a = 3; }");
    by_line = append(by_line, TOKEN, expression_token);
    expect_data = append( expect_data, ARRAY, by_line );


    int counter = 0;
    while ( testing_data[counter] ) {
        Array ** result_data = new_array();
        char * input_stream = alloc_string(testing_data[counter]);
        result_data = extract_token(input_stream, result_data);
        result *= array_cmp(result_data, expect_data[counter]->element);

        printf("input: `%s` -- %s => ", testing_data[counter], result ? "OK" : "FAIL");
        display_array(result_data);
        printf("\n");

        counter++;
    }
    return result;
}

int test_is_keyword() {
    int result = 1;
    printf("\ntest_is_keyword:\n");

    char * testing_data[] = {
            "function",
            "if",
            "else",
            "while",
            "return",
            "namespace",
            "global",
            "continue",
            0
    };

    int expect_data[] = {
        LEXER_FUNCTION_TK,
        LEXER_IF_TK,
        LEXER_ELSE_TK,
        LEXER_WHILE_TK,
        LEXER_RETURN_TK,
        LEXER_NAMESPACE_TK,
        LEXER_GLOBAL_TK,
        LEXER_CONTINUE_TK
    };

    int counter = 0;
    while ( testing_data[counter] ) {
        char * input_stream = alloc_string( testing_data[counter] );
        int result_data = is_keyword(input_stream);
        result *= result_data == expect_data[counter];

        printf("input: `%s` -- %s => / %d /\n", testing_data[counter], result ? "OK" : "FAIL", result_data);
        counter++;
    }

    return result;
}

int test_cut_token() {
    int result = 1;
    printf("\ntest_cut_token:\n");

    char * testing_data[] = {
            "3.13 + 2",
            "+2  ",
            "---",
            0,
    };

    char * expect_data[] = {
            "3.13",
            "+2",
            "---",
            0,
    };

    int counter = 0;
    while ( testing_data[counter] ) {
        char * input_stream = alloc_string( testing_data[counter] );
        char * result_string = cut_token(input_stream, OP_SPACE);
        result *= !strcmp(expect_data[counter], result_string);

        printf("input: `%s` -- %s => / %s /\n", testing_data[counter], result ? "OK" : "FAIL", result_string);
        counter++;
    }

    return result;
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
Constant * wrapper(char * input_stream) {
    // Initialize the call stack array
    call_stack = new_array(); call_stack = append(call_stack, CALLSTACK_POINT, new_call_stack_point("__MAIN__", INTERPRETER_CALL_STACK_MAIN));
    // Initialize the heap array, that contained the addresses */
    memory_table = new_array();

    Constant * result;
    /* parsing the input text stream */
    Array ** parsed = main_parsing(input_stream);

    /* initialization the namespace of interpreter */
    global_namespace = meta_data();

    /* interpreting the program in a global_namespace */
    result = interpreter(parsed, global_namespace); (result == NULL) ? result = new_constant(NONE, NULL) : NULL;
    display_memory_table(memory_table);
    return result;
}

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
                    case TOKEN:
                        return
                            ((Token *)(a[counter]->element))->type_id == ((Token *)(b[counter]->element))->type_id &&
                            !strcmp((char *)(((Token *)(a[counter]->element))->value), (char *)(((Token *)(a[counter]->element))->value));
                        break;
                }

            }
            counter++;
        }
    }
    return 1;
}

/* Arrays */