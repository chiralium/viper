#include "builtins.h"
#include "functions.h"
#include "ViArray.h"
#include "memory.h"
#include "composer.h"
#include "arithmetica.h"

#include <unistd.h>

extern Array ** memory_table;
extern char * expression_as_string;
Node * namespace;

BuiltIn * new_builtin(char * name, void * function_pointer, int args) {
    BuiltIn * builtin = malloc(sizeof(BuiltIn));
    builtin->name = name;
    builtin->function_pointer = function_pointer;
    builtin->args = args;
    return builtin;
}

BuiltIn builtin_timeout = { BT_TIMEOUT, timeout, 2 };
Constant * timeout( Constant * callback, Constant * delay ) {
    int args_is_valid = callback->type_id == FUNCTION_CONTAINER && delay->type_id == INTEGER;
    if ( !args_is_valid ) throw_typecasting_exception(expression_as_string, BUILTIN_FUNCTION_SETTIMEOUT_INVALID_TYPE);
    sleep(*(int *)delay->value);
    return callback_precalc(callback, new_array());
}

BuiltIn builtin_output = { BT_OUTPUT, output, 1 };
Constant * output(Constant * value) {
    switch (value->type_id) {
        /* Vipers types */
        case NONE:
            printf("<NONE>");
            break;
        case INTEGER:
            printf("%d", *(int *) value->value);
            break;
        case FLOAT:
            printf("%f", *(float *) value->value);
            break;
        case STRING:
            printf("%s", (char *) value->value);
            break;
        case VIARRAY:
            printf("{");
            display_viarray(value->value);
            printf("}");
            break;
        case FUNCTION_CONTAINER:
            printf("<function `%s` at [0x%p]>",
                   ((FunctionContainer *) (value->value))->name,
                   value->value);
            break;
        case BUILT_IN_FUNCTION:
            printf("<built-in function `%s` at [0x%p]>",
                   ((BuiltIn *)(value->value))->name, value->value);
            break;
        case NAMESPACE:
            printf("<namespace `%s` at [0x%p]>",
                   ((NameSpaceObject *) (value->value))->name,
                   value->value);
            break;
        default:
            printf("<undefined at [0x%p]>", value->value);
    }
    return new_constant(NONE, NULL);
}

BuiltIn builtin_input = { BT_INPUT, input, 1 };
Constant * input(Constant * value) {
    char input_buffer[ARITHMETICA_MAX_STRING_LEN];
    if (value->type_id != STRING) throw_typecasting_exception(expression_as_string, BUILTIN_FUNCTION_INPUT_INVALID_TYPE);
    printf("%s", value->value); gets(input_buffer);
    return new_constant(STRING, alloc_string(input_buffer));
}

BuiltIn builtin_len = { BT_LEN, len, 1 };
Constant * len(Constant * value) {
    if (value->type_id != VIARRAY && value->type_id != STRING) throw_typecasting_exception(expression_as_string, BUILTIN_FUNCTION_LEN_INVALID_TYPE);
    int * length = malloc(sizeof(int));
    if (value->type_id == VIARRAY) *length = get_length(value->value);
    else if (value->type_id == STRING) *length = strlen(value->value);
    return new_constant(INTEGER, length);
}

BuiltIn builtin_to_string = { BT_STRING, to_string, 1 };
Constant * to_string(Constant * value) {
    char str[ARITHMETICA_MAX_STRING_LEN];
    if (value->type_id == INTEGER) snprintf(str, sizeof(str), "%d", *(int *)value->value);
    else if (value->type_id == FLOAT) snprintf(str, sizeof(str), "%f", *(float *)value->value);
    else if (value->type_id == STRING) strcpy(str, value->value);
    else if (value->type_id == NONE) strcpy(str, "NONE");
    else throw_typecasting_exception(expression_as_string, BUILTIN_FUNCTION_TO_STRING_INVALID_TYPE);

    return new_constant(STRING, alloc_string(str));
}

BuiltIn builtin_to_int = { BT_INTEGER, to_int, 1 };
Constant * to_int(Constant * value) {
    int * integer = malloc(sizeof(int));
    if (value->type_id == INTEGER) *integer = *(int *)value->value;
    else if (value->type_id == FLOAT) *integer = *(float *)value->value;
    else if (value->type_id == STRING) {
        if (is_int_number(value->value)) *integer = atoi(value->value);
        else if (is_float_number(value->value)) *integer = atof(value->value);
        else throw_typecasting_exception(expression_as_string, BUILTIN_FUNCTION_TO_INTEGER_SYNTAX_ERROR);
    } else throw_typecasting_exception(expression_as_string, BUILTIN_FUNCTION_TO_INTEGER_INVALID_TYPE);
    return new_constant(INTEGER, integer);
}

BuiltIn builtin_to_float = { BT_FLOAT, to_float, 1 };
Constant * to_float(Constant * value) {
    float * integer = malloc(sizeof(float));
    if (value->type_id == INTEGER) *integer = *(int *)value->value;
    else if (value->type_id == FLOAT) *integer = *(float *)value->value;
    else if (value->type_id == STRING) {
        if (is_int_number(value->value)) *integer = atoi(value->value);
        else if (is_float_number(value->value)) *integer = atof(value->value);
        else throw_typecasting_exception(expression_as_string, BUILTIN_FUNCTION_TO_INTEGER_SYNTAX_ERROR);
    } else throw_typecasting_exception(expression_as_string, BUILTIN_FUNCTION_TO_INTEGER_INVALID_TYPE);
    return new_constant(FLOAT, integer);
}

BuiltIn * builtins[100] = {&builtin_input,
                           &builtin_output,
                           &builtin_len,
                           &builtin_to_string,
                           &builtin_to_int,
                           &builtin_to_float,
                           &builtin_timeout};

Constant * builtin_function_execute(BuiltIn * builtin, Array ** input_args) {
    int signature = builtin->args;
    if (signature == 1) {
        Constant * (*function_pointer)(Constant *) = builtin->function_pointer;
        Constant * first_arg = input_args[0]->element;
        Constant * value = function_pointer(first_arg);

        is_simple_data(first_arg->type_id) ? constant_destructor(first_arg) : free(first_arg);
        free(input_args[0]); free(input_args);
        return value;
    } else if ( signature == 2 ) {
        Constant * (*function_pointer)(Constant *, Constant *) = builtin->function_pointer;
        Constant * arg_list[] = {
                input_args[0]->element,
                input_args[1]->element
        };
        Constant * value = function_pointer(arg_list[0], arg_list[1]);

        while ( --signature ) {
            is_simple_data(arg_list[signature]->type_id) ?
            constant_destructor(arg_list[signature]) :
            free(arg_list[signature]);
            free(input_args[signature]);
        }
        free(input_args);
        return value;
    }
}

void import_builtins(Node * namespace) {
    int counter = 0;
    while (builtins[counter]) {
        BuiltIn * builtin = new_builtin(alloc_string(builtins[counter]->name),
                                        builtins[counter]->function_pointer,
                                        builtins[counter]->args);
        Node * node = new_node(faq6(builtin->name), new_constant(BUILT_IN_FUNCTION, builtin));
        insert_node(namespace, node); memory_table = append(memory_table, MEMORY_ELEMENT, new_memory_element(BUILT_IN_FUNCTION, builtin, "builtins.c"));
        counter++;
    }
}

