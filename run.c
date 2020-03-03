#include "run.h"

void run(void) {
    char *filename = "C:\\viper\\viper-v4\\cmake-build-debug\\test.vi";
    char *input_stream; input_stream = readfile(filename);
    Array ** literals = recursive_descent(input_stream); free(input_stream);
    printf("\nCode structure: \n\n");
    display_array(literals);

    printf("\nTokens: \n\n");
    Array ** tokens = lexer(literals);
    display_array(tokens);

    printf("\nParsed tokens: \n\n");
    Array ** parsed_tokens = parser(tokens);
    display_array(parsed_tokens);

    array_destructor(parsed_tokens);
}
