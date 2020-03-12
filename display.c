#include "display.h"

void display_array_beauty(Array ** _array, char * tabs) {
    strcat(tabs, "-");
    printf("%s[\n", tabs);
    if (!_array) printf("]");
    int array_counter = 0;
    while (_array[array_counter]) {
        if (_array[array_counter] -> type_id == INTEGER) printf("`%d`, ", *(int *)_array[array_counter] -> element);
        else if (_array[array_counter] -> type_id == FLOAT) printf("`%d`, ", *(float *)_array[array_counter] -> element);
        else if (_array[array_counter] -> type_id == STRING) printf("%s%s`%s`, \n", tabs, tabs, (char *)_array[array_counter] -> element);
        else if (_array[array_counter] -> type_id == TOKEN) printf ("%s%s{TK:%d; `%s`}, \n", tabs, tabs, ((Token *)(_array[array_counter] -> element)) -> type_id,
                                                                                                 (char *)((Token *)(_array[array_counter] -> element)) -> value);
        else if (_array[array_counter] -> type_id == NULL_TOKEN) printf("%s%s{TK:NULL}, \n", tabs, tabs);

        else if (_array[array_counter] -> type_id == STMT_IF) {
            printf("\n%s%s{STMT_IF; `%s`}: \n", tabs, tabs, ((If *)(_array[array_counter] -> element)) -> condition);
            display_statements(_array[array_counter] -> element, 1, tabs);
            printf("\n\n");
        } else if (_array[array_counter] -> type_id == STMT_FUNC) {
            printf("%s%s{STMT_FUNC; `%s`}: \n", tabs, tabs, ((Function *)(_array[array_counter] -> element)) -> name);
            display_statements(_array[array_counter] -> element, 2, tabs);
            printf("\n\n");
        } else if (_array[array_counter] -> type_id == STMT_WHILE) {
            printf("\n%s%s{STMT_WHILE; `%s`}: \n", tabs, tabs, ((While *) (_array[array_counter]->element))->condition);
            display_statements(_array[array_counter] -> element, 3, tabs);
            printf("\n\n");
        } else if (_array[array_counter] -> type_id == EXP_TK) {
            char token_type_id = ((ExpressionToken *)(_array[array_counter] -> element)) -> type_id;
            char * token_literal = ((ExpressionToken *)(_array[array_counter] -> element)) -> literal;
            char token_value_type_id = ((ExpressionToken *)(_array[array_counter] -> element)) -> vtype_id;
            void * token_value = ((ExpressionToken *)(_array[array_counter] -> element)) -> value;


            switch(token_value_type_id) {
                case ITERATOR:
                    printf("%s%s{`<iterator>`}:\n", tabs, tabs, token_type_id);
                    char child_tabs_iterator[255]; strcpy(child_tabs_iterator, tabs); strcat(child_tabs_iterator, tabs);
                    display_iterator(token_value, child_tabs_iterator);
                    break;
                case STRING:
                    printf("%s%s{`%s`:<%c>(0x%p)}, \n", tabs, tabs, (char *)token_value, token_value_type_id, token_value);
                    break;
                case ARRAY:
                    printf("%s%s{`<array>`}:\n", tabs, tabs, token_type_id);
                    char child_tabs[255]; strcpy(child_tabs, tabs); strcat(child_tabs, tabs);
                    display_array_beauty(token_value, child_tabs);
                    break;
                case INTEGER:
                    printf("%s%s{`%d`:<%c>(0x%p)}, \n", tabs, tabs, *(int *)token_value, token_value_type_id, token_value);
                    break;
                case FLOAT:
                    printf("%s%s{`%f`:<%c>(0x%p)}, \n", tabs, tabs, *(float *)token_value, token_value_type_id, token_value);
                    break;
                case FUNCTION:
                    printf("%s%s{`function`:<%s> (0x%p)}, \n", tabs, tabs, token_literal, token_value);
                    break;
                default:
                    printf("%s%s{`%s`:<%c>(0x%p)}, \n", tabs, tabs, token_literal, token_value_type_id, token_value);
            }
        } else if (_array[array_counter] -> type_id == COMPLEX_TOKEN) printf("%s%s{TK:%d; `<complex>`}, \n", tabs, tabs, ((Token *)(_array[array_counter] -> element)) -> type_id);
        else if (_array[array_counter] -> type_id == ARRAY) {
            char child_tabs[255]; strcpy(child_tabs, tabs);
            display_array_beauty((Array **)(_array[array_counter] -> element), child_tabs);
        }
        array_counter++;
    }
    printf("%s]\n", tabs);
}

void display_array(Array ** _array) {
    printf("[");
    if (!_array) printf("]");
    int array_counter = 0;
    while (_array[array_counter]) {
        if (_array[array_counter] -> type_id == INTEGER) printf("`%d`, ", *(int *)_array[array_counter] -> element);
        else if (_array[array_counter] -> type_id == FLOAT) printf("`%d`, ", *(float *)_array[array_counter] -> element);
        else if (_array[array_counter] -> type_id == STRING) printf("`%s`, ", (char *)_array[array_counter] -> element);
        else if (_array[array_counter] -> type_id == TOKEN) printf ("{TK:%d; `%s`}, ", ((Token *)(_array[array_counter] -> element)) -> type_id,
                                                                                       (char *)((Token *)(_array[array_counter] -> element)) -> value);
        else if (_array[array_counter] -> type_id == NULL_TOKEN) printf("{TK:NULL}, ");

        else if (_array[array_counter] -> type_id == STMT_IF) printf("{STMT_IF; `%s`}, ", ((If *)(_array[array_counter] -> element)) -> condition);
        else if (_array[array_counter] -> type_id == STMT_FUNC) printf("{STMT_FUNC; `%s`}, ", ((Function *)(_array[array_counter] -> element)) -> name);
        else if (_array[array_counter] -> type_id == STMT_WHILE) printf("{STMT_WHILE; `%s`}, ", ((While *)(_array[array_counter] -> element)) -> condition);

        else if (_array[array_counter] -> type_id == EXP_TK) {
            char token_type_id = ((ExpressionToken *)(_array[array_counter] -> element)) -> type_id;
            char * token_literal = ((ExpressionToken *)(_array[array_counter] -> element)) -> literal;
            char token_value_type_id = ((ExpressionToken *)(_array[array_counter] -> element)) -> vtype_id;
            void * token_value = ((ExpressionToken *)(_array[array_counter] -> element)) -> value;

            switch(token_value_type_id) {
                case STRING:
                    printf("{`%s`:<%c>(0x%p)}, ", (char *)token_value, token_value_type_id, token_value);
                    break;
                case ARRAY:
                    printf("{`<array>`: ", token_type_id);
                    display_array(token_value);
                    printf("}, ");
                    break;
                case INTEGER:
                    printf("{`%d`:<%c>(0x%p)}, ", *(int *)token_value, token_value_type_id, token_value);
                    break;
                case FLOAT:
                    printf("{`%f`:<%c>(0x%p)}, ", *(float *)token_value, token_value_type_id, token_value);
                    break;
                case FUNCTION:
                    printf("{`function`:<%s> (0x%p)}, ", token_literal, token_value);
                    break;
                default:
                    printf("{`%s`:<%c>(0x%p)}, ", token_literal, token_value_type_id, token_value);
            }
        } else if (_array[array_counter] -> type_id == COMPLEX_TOKEN) printf("{TK:%d; `<complex>`}, ", ((Token *)(_array[array_counter] -> element)) -> type_id);
        else if (_array[array_counter] -> type_id == ARRAY) {
            display_array((Array **)(_array[array_counter] -> element)); printf(", ");
        }
        array_counter++;
    }
    printf("]");
}

void display_statements(void * statement, char type_id, char tabs[255]) {
    If * if_statement;
    Function * function_statement;
    While * while_statement;
    switch (type_id) {
        case 1:
            /* if-else statement */
            if_statement = statement;
            printf("%s%s%sIF `%s`: ", tabs, tabs, tabs, if_statement->condition); display_array(if_statement->body);
            if (if_statement->else_condition) display_statements(if_statement->else_condition, 4, tabs);
            break;
        case 2:
            /* function statement */
            function_statement = statement;
            printf("%s%s%sFUNCTION `%s` ARG: ", tabs, tabs, tabs, function_statement->name);
            if (function_statement->arg_list) {
                display_array(function_statement->arg_list);
            }
            else printf("[empty]\n");
            printf("\n%s%s%sBODY:\n", tabs, tabs, tabs);
            char child_tabs[255]; strcpy(child_tabs, tabs); strcat(child_tabs, tabs);
            display_array_beauty(function_statement->body, child_tabs);
            break;

        case 3:
            /* while-statement */
            while_statement = statement;
            printf("%s%s%sWHILE `%s`: ", tabs, tabs, tabs, while_statement->condition); display_array(while_statement->body);
            break;

        case 4:
            /* else-tails */
            if_statement = statement;
            if (if_statement->condition) {
                printf("\n%s%s%s[ELSE] `%s`: ", tabs, tabs, tabs, if_statement->condition);
                display_array(if_statement->body);
            } else {
                printf("\n%s%s%s[ELSE]: ", tabs, tabs, tabs);
                display_array(if_statement->body);
            }

            if (if_statement->else_condition) display_statements(if_statement->else_condition, 4, tabs);
            break;
    }
}

void display_iterator(Iterator * iterator, char tabs[255]) {
    printf("%s%s%sITERATOR:\n", tabs, tabs, tabs);
    printf("%s%s%s%sOBJECT:\n", tabs, tabs, tabs, tabs); {
        char child_tabs[255]; strcpy(child_tabs, tabs); strcat(child_tabs, tabs); strcat(child_tabs, tabs); strcat(child_tabs, tabs);
        display_array_beauty(iterator->object, child_tabs);
    }
    printf("%s%s%s%sSTART:", tabs, tabs, tabs, tabs); (iterator->start) ? display_array(iterator->start) : printf("<null>"); printf("\n");
    printf("%s%s%s%sSTOP:", tabs, tabs, tabs, tabs); (iterator->stop) ? display_array(iterator->stop) : printf("<null>"); printf("\n");
    printf("%s%s%s%sSTEP:", tabs, tabs, tabs, tabs); (iterator->step) ? display_array(iterator->step) : printf("<null>"); printf("\n");
}