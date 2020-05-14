#include "builtins.h"
#include "functions.h"
#include "ViArray.h"
#include "memory.h"

extern Array ** memory_table;

BuiltIn * new_builtin(char * name, void * function_pointer, int args) {
    BuiltIn * builtin = malloc(sizeof(BuiltIn));
    builtin->name = name;
    builtin->function_pointer = function_pointer;
    builtin->args = args;
    return builtin;
}

BuiltIn builtin_output = {"output", output, 1};
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
    (is_simple_data(value->type_id)) ? free(value->value) : NULL;
    free(value);
    return new_constant(NONE, NULL);
}

BuiltIn builtin_input = {"input", input, 1};
Constant * input(Constant * value) {
    char input_buffer[ARITHMETICA_MAX_STRING_LEN];
    output(value); gets(input_buffer);
    return new_constant(STRING, alloc_string(input_buffer));
}

BuiltIn * builtins[100] = {&builtin_input,
                           &builtin_output};

Constant * builtin_function_execute(BuiltIn * builtin, Array ** input_args) {
    int signature = builtin->args;
    if (signature == 1) {
        Constant * (*function_pointer)(Constant *) = builtin->function_pointer;
        Constant * value = function_pointer((Constant *)(input_args[0]->element));
        free(input_args[0]); free(input_args);
        return value;
    } else if (signature == 2) {
        Constant * (*function_pointer)(Constant *, Constant *) = builtin->function_pointer;
        return function_pointer((Constant *)(input_args[0]->element), (Constant *)(input_args[1]->element));
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

