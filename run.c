#include "run.h"

void run(void) {
    char *filename = "C:\\viper\\viper-v4\\cmake-build-debug\\test.vi";
    char *input_stream; input_stream = readfile(filename);

    Array ** literals = recursive_descent(input_stream); free(input_stream);
    printf("\n\nCode structure: \n");
    display_array(literals);

    printf("\n\nTokens: \n");
    Array ** tokens = lexer(literals);
    display_array(tokens);

    printf("\n\nParsed:\n");
    Array ** parsed_tokens = parser(tokens);
    display_array(parsed_tokens);

    printf("\n\nExpression Tokens:");
    Array ** expression_tokens = expression_lexer(parsed_tokens);
    display_array(expression_tokens);

    array_destructor(literals); array_destructor(tokens); parsed_token_destructor(parsed_tokens);
}
