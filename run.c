#include "run.h"

void run(void) {
    char *filename = "test.vi";
    char *input_stream; input_stream = readfile(filename);
    Array ** literals = recursive_descent(input_stream);
    display_array(literals);
    array_destructor(literals);
}
