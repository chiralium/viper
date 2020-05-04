#include <windows.h>
#include "display.h"
#include "ViArray.h"
#include "functions.h"
#include "interpreter.h"

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
            display_statements(_array[array_counter]->element, 3, tabs);
            printf("\n\n");
        } else if (_array[array_counter] -> type_id == STMT_RETURN) {
            printf("\n%s%s{STMT_RETURN; `%s`}: \n", tabs, tabs, ((Return *) (_array[array_counter]->element))->expression);
            printf("\n\n");
        } else if (_array[array_counter] -> type_id == STMT_NAMESPACE) {
            printf("\n%s%s{STMT_NAMESPACE; `%s`: \n", tabs, tabs, ((NameSpace *)(_array[array_counter]->element))->name);
            display_statements(_array[array_counter]->element, 5, tabs);
            printf("\n\n");
        } else if (_array[array_counter] -> type_id == EXP_TK) {
            char token_type_id = ((ExpressionToken *)(_array[array_counter] -> element)) -> type_id;
            char * token_literal = ((ExpressionToken *)(_array[array_counter] -> element)) -> literal;
            char token_value_type_id = ((ExpressionToken *)(_array[array_counter] -> element)) -> vtype_id;
            void * token_value = ((ExpressionToken *)(_array[array_counter] -> element)) -> value;


            switch(token_value_type_id) {
                case UNDEFINED:
                    printf("%s%s{`%s`:<u>} \n", tabs, tabs, token_literal);
                    break;
                case INDEX:
                    printf("%s%s{`<index>`}:\n", tabs, tabs, token_type_id);
                    char child_tabs_index[512]; strcpy(child_tabs_index, tabs);  strcat(child_tabs_index, tabs); strcat(child_tabs_index, tabs);
                    display_index(token_value, child_tabs_index);
                    break;
                case STRING:
                    printf("%s%s{`%s`:<%c>(0x%p)}, \n", tabs, tabs, (char *)token_value, token_value_type_id, token_value);
                    break;
                case ARRAY_EL: case ARRAY:
                    printf("%s%s{`<array>`}:\n", tabs, tabs, token_type_id);
                    char child_tabs[512]; strcpy(child_tabs, tabs); strcat(child_tabs, tabs);
                    display_array_beauty(token_value, child_tabs);
                    break;
                case INTEGER:
                    printf("%s%s{`%d`:<%c>(0x%p)}, \n", tabs, tabs, *(int *)token_value, token_value_type_id, token_value);
                    break;
                case FLOAT:
                    printf("%s%s{`%f`:<%c>(0x%p)}, \n", tabs, tabs, *(float *)token_value, token_value_type_id, token_value);
                    break;
                case FUNCTION_RES:
                    printf("%s%s{`function`}:\n", tabs,tabs);
                    char child_tabs_function_call[512]; strcpy(child_tabs_function_call, tabs); strcat(child_tabs_function_call, tabs);
                    display_array_beauty(((FuncCall *)token_value)->function_pointer, child_tabs_function_call);
                    printf("%s%sarg_list:\n", tabs, tabs);
                    *child_tabs_function_call = '\0'; strcpy(child_tabs_function_call, tabs); strcat(child_tabs_function_call, tabs);
                    display_array_beauty(((FuncCall *)token_value)->arg_list, child_tabs_function_call);
                    break;
                default:
                    printf("%s%s{`operator`:<%s> (0x%p)}, \n", tabs, tabs, token_literal, token_value);
                    break;
            }
        } else if (_array[array_counter] -> type_id == COMPLEX_TOKEN) printf("%s%s{TK:%d; `<complex>`}, \n", tabs, tabs, ((Token *)(_array[array_counter] -> element)) -> type_id);
        else if (_array[array_counter] -> type_id == ARRAY || _array[array_counter] -> type_id == ARRAY_EL) {
            char child_tabs[512]; strcpy(child_tabs, tabs);
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
        if (_array[array_counter] -> type_id == INTEGER) printf("%d, ", *(int *)_array[array_counter] -> element);
        else if (_array[array_counter] -> type_id == FLOAT) printf("%f, ", *(float *)_array[array_counter] -> element);
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
                    printf("{`<array>`: ");
                    display_array(token_value);
                    printf("}, ");
                    break;
                case INTEGER:
                    printf("{`%d`:<%c>(0x%p)}, ", *(int *)token_value, token_value_type_id, token_value);
                    break;
                case FLOAT:
                    printf("{`%f`:<%c>(0x%p)}, ", *(float *)token_value, token_value_type_id, token_value);
                    break;
                case OPERATOR:
                    printf("{`operator`:<%s> (0x%p)}, ", token_literal, token_value);
                    break;
                case INDEX:
                    printf("{`index`}, ");
                    break;
                case FUNCTION_RES:
                    printf("{`function`}");
                    break;
                default:
                    printf("{`operator`:<%s> (0x%p)}, ", token_literal, token_value);
                    break;
            }
        } else if (_array[array_counter] -> type_id == COMPLEX_TOKEN) printf("{TK:%d; `<complex>`}, ", ((Token *)(_array[array_counter] -> element)) -> type_id);
        else if (_array[array_counter] -> type_id == ARRAY) {
            display_array((Array **)(_array[array_counter] -> element)); printf(", ");
        }
        array_counter++;
    }
    printf("]");
}

void display_statements(void * statement, char type_id, char tabs[512]) {
    If * if_statement;
    Function * function_statement;
    While * while_statement;
    NameSpace * namespace_statement;
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
            printf("%s%s%sFUNCTION `%s` NS `%s` ARG: ", tabs, tabs, tabs, function_statement->name, (function_statement->namespace == NULL) ? "NONE" : function_statement->namespace);
            if (function_statement->arg_list) {
                display_array(function_statement->arg_list);
            }
            else printf("[empty]\n");
            printf("\n%s%s%sBODY:\n", tabs, tabs, tabs);
            char child_tabs[512]; strcpy(child_tabs, tabs); strcat(child_tabs, tabs);
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
        case 5:
            /* namespace */
            namespace_statement = statement;
            printf("%s%s%sNAMESPACE `%s`", tabs, tabs, tabs, namespace_statement->name);
            printf("\n%s%s%sBODY:\n", tabs, tabs, tabs);
            char child_tabs_ns[512]; strcpy(child_tabs_ns, tabs); strcat(child_tabs_ns, tabs);
            display_array_beauty(namespace_statement->body, child_tabs_ns);
            break;
    }
}

void display_index(Index * index, char tabs[512]) {

    printf("%sINDEX:\n", tabs);
    printf("%s%sOBJECT:\n", tabs, tabs);

    char child_tabs[512];
    strcpy(child_tabs, tabs);
    strcat(child_tabs, tabs);

    Array ** object = index->object;

    display_array_beauty(object, child_tabs);

    printf("%s%sSTART:", tabs, tabs);
    if (index->params != NULL) {
        display_array_beauty((index->params[0])->element, child_tabs); printf("\n");
    } else printf("%s<NEW_ELEMENT>\n", child_tabs);

    if (index->params_count == 3) {
        printf("%s%sSTOP:", tabs, tabs); display_array((index->params[1])->element); printf("\n");
        printf("%s%sSTEP:", tabs, tabs); display_array((index->params[2])->element); printf("\n");
    } else if (index->params_count == 2) {
        printf("%s%sSTOP:", tabs, tabs); display_array((index->params[1])->element); printf("\n");
        printf("%s%sSTEP:", tabs, tabs); printf("<null>"); printf("\n");
    } else if (index->params_count == 1) {
        printf("%s%sSTOP:", tabs, tabs); printf("<null>"); printf("\n");
        printf("%s%sSTEP:", tabs, tabs); printf("<null>"); printf("\n");
    }
}

void display_constant(Constant * constant) {
    switch (constant->type_id) {
        /* Vipers types */
        case NONE:
            printf("<NONE>");
            break;
        case INTEGER:
            printf("%d", *(int *)constant->value);
            break;
        case FLOAT:
            printf("%f", *(float *)constant->value);
            break;
        case STRING:
            printf("`%s`", (char *)constant->value);
            break;
        case VIARRAY:
            printf("{"); display_viarray(constant->value); printf("}");
            break;
        case FUNCTION_CONTAINER:
            printf("<function `%s` at [0x%p]>",
                   ((FunctionContainer *)(constant->value))->name,
                   constant->value);
            break;
        case NAMESPACE:
            printf("<namespace `%s` at [0x%p]>",
                   ((NameSpaceObject *)(constant->value))->name,
                   constant->value);
            break;
        /* Systems types */
        case ARRAY:
            printf("\n<array>: "); display_array(constant->value); printf("\n");
            break;
        case INDEX:
            printf("\n<index>: "); display_index(constant->value, " "); printf("\n");
            break;
        case FUNCTION_RES:
            printf("\n<function>: "); display_function(constant->value, " "); printf("\n");
            break;
        case KEYPAIR:
            display_node(constant->value);
            break;
    }
}

void display_function(FuncCall * function, char tabs[512]) {
    printf("<function>: \n");
    display_array_beauty(function->function_pointer, tabs);
    display_array_beauty(function->arg_list, tabs);
}

void display_callstack(Array ** points) {
    set_color_scheme(COLOR_SCHEME_CALL_STACK_HEAD);
    printf("CallStack: ");
    set_color_scheme(COLOR_SCHEME_CALL_STACK_TAIL);
    while (*points) {
        CallStackPoint * point = (*points)->element;
        printf("-> %s ", point->label);
        points++;
    }
    set_color_scheme(-1);
}

void set_color_scheme(int scheme) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    switch (scheme) {
        case COLOR_SCHEME_WARNING: SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE); break;
        case COLOR_SCHEME_EXCEPTION: SetConsoleTextAttribute(hConsole, FOREGROUND_RED); break;
        case COLOR_SCHEME_CALL_STACK_HEAD: SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE); break;
        case COLOR_SCHEME_CALL_STACK_TAIL: SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY); break;
        case COLOR_SCHEME_INTERPRETER_OUTPUT: SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE); break;
        case COLOR_SCHEME_MEMORY_TABLE: SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN); break;
        case COLOR_SCHEME_DEFAULT: SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN); break;
        default: SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
    }
}