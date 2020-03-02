#include "run.h"

void run(void) {
    char *filename = "C:\\viper\\viper-v4\\cmake-build-debug\\test.vi";
    char *input_stream; input_stream = readfile(filename);
    Array ** literals = recursive_descent(input_stream); free(input_stream);
    printf("Code structure: \n");
    display_array(literals);

    printf("\nTokens: \n");
    Array ** tokens = lexer(literals);
    display_array(tokens);
    array_destructor(literals); array_destructor(tokens);
}
