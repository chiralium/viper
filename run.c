#include "run.h"

void run(void) {
    char *filename = "C:\\viper\\viper-v4\\cmake-build-debug\\test.vi";
    char *input_stream; input_stream = readfile(filename);
    char tabs[255];

    Array ** literals = recursive_descent(input_stream); free(input_stream);
    printf("\n\nCode structure: \n");
    display_array_beauty(literals, tabs);
    tabs[0] = '\0';

    printf("\n\nTokens: \n");
    Array ** tokens = lexer(literals);
    display_array_beauty(tokens, tabs);
    tabs[0] = '\0';

    printf("\n\nParsed:\n");
    Array ** parsed_tokens = parser(tokens);
    display_array_beauty(parsed_tokens, tabs);
    tabs[0] = '\0';

    printf("\n\nExpression Tokens:\n");
    Array ** expression_tokens = expression_lexer(parsed_tokens);
    display_array_beauty(expression_tokens, tabs);

    array_destructor(literals);
    array_destructor(tokens);
    parsed_token_destructor(parsed_tokens);
    exp_tokens_destructor(expression_tokens);
}
