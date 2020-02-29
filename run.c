#include "run.h"

void run(void) {
    char *filename = "test.vi";
    char *input_stream; input_stream = readfile(filename);
    Array ** literals = recursive_descent(input_stream);
    printf("Code structure: \n");
    display_array(literals);
    printf("\nTokens: \n");
    Array ** tokens = lexer(literals);
    display_array(tokens);
}
