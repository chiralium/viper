#include "run.h"

Array ** call_stack;
Array ** memory_table;

void run(void) {
    // Read the source file
    char *filename = "/home/evgeniy/computer/lab/viper/cmake-build-debug/test.vi"; char *input_stream; input_stream = readfile(filename);

    // Initialize the call stack array
    call_stack = new_array(); call_stack = append(call_stack, CALLSTACK_POINT, new_call_stack_point("__MAIN__", INTERPRETER_CALL_STACK_MAIN));

    // Initialize the heap array, that contained the addresses */
    memory_table = new_array();

    // Start the program
    main_entry(input_stream);
    array_destructor(call_stack);
    memory_table_destructor(memory_table);
}
