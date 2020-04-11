#include "run.h"

void run(void) {
    char *filename = "C:\\viper\\viper-v4\\cmake-build-debug\\test.vi";
    char *input_stream; input_stream = readfile(filename);

    // call stack is a global array
    call_stack = new_array();
    call_stack = append(call_stack, STRING, alloc_string("__MAIN__"));
    parsing(input_stream);
    array_destructor(call_stack);
}
