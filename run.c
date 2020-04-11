#include "run.h"

void run(void) {
    // Read the source file
    char *filename = "C:\\viper\\viper-v4\\cmake-build-debug\\test.vi"; char *input_stream; input_stream = readfile(filename);

    // Initialize the call stack array
    call_stack = new_array(); call_stack = append(call_stack, STRING, alloc_string("__MAIN__"));

    // Start the program
    main_entry(input_stream); array_destructor(call_stack);
}
