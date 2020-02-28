#include "run.h"

void run(void) {
    char *filename = "test.vi";
    char *input_stream; input_stream = readfile(filename);
    Array ** structure = recursive_descent(input_stream);
    display_array(structure);
    array_destructor(structure);
}
