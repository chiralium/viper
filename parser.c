#include "parser.h"

Array ** cut_tokens(Array ** tokens, int stop_id) {
    Array ** cuted_tokens = new_array(); Token * token;
    while (token = (Token *)(get_first_el(tokens) -> element)) {
        if (token->type_id == stop_id) break;
        else {
            Array * cuted_token = pop_el(tokens);
            cuted_tokens = append(cuted_tokens, cuted_token->type_id, (Token *)(cuted_token -> element));
        }
    }
    return cuted_tokens;
}

char * trim(char * literal) {
    while (get_first(literal) == FPARSER_SPACE) pop_first(literal); // left side cutter
    while (get_last(literal) == FPARSER_SPACE) pop_last(literal);   // right side cutter
    return literal;
}