#include "display.h"

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

        else if (_array[array_counter] -> type_id == EXP_TK) printf("{EXP_TK:%d; `%s` <%c>(0x%p)}, ", ((ExpressionToken *)(_array[array_counter] -> element)) -> type_id,
                                                                                                      ((ExpressionToken *)(_array[array_counter] -> element)) -> literal,
                                                                                                      ((ExpressionToken *)(_array[array_counter] -> element)) -> vtype_id,
                                                                                                      ((ExpressionToken *)(_array[array_counter] -> element)) -> value);

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
    Function * function_statement;
    While * while_statement;
    switch (type_id) {
        case 1:
            /* if-else statement */
            if_statement = statement;
            printf("\nIF `%s` ", if_statement->condition); display_array(if_statement->body);
            if (if_statement->else_condition) display_statements(if_statement->else_condition, 4);
            break;
        case 2:
            /* function statement */
            function_statement = statement;
            printf("\nFUNCTION `%s` ARG: ", function_statement->name);
            if (function_statement->arg_list) display_array(function_statement->arg_list);
            else printf("[empty]");
            printf("\nBODY: ");
            display_array(function_statement->body);
            break;

        case 3:
            /* while-statement */
            while_statement = statement;
            printf("\nWHILE `%s` ", while_statement->condition); display_array(while_statement->body);
            break;

        case 4:
            /* else-tails */
            if_statement = statement;
            if (if_statement->condition) {
                printf(" [ELSE] `%s`", if_statement->condition); display_array(if_statement->body);
            } else printf(" [ELSE] "); display_array(if_statement->body);

            if (if_statement->else_condition) display_statements(if_statement->else_condition, 4);
            break;
    }
}