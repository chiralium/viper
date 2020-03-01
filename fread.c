#include "fread.h"

unsigned long get_size(FILE *f) {
    unsigned long size = 0;
    while (getc(f) != EOF) size++;
    fseek(f, 0, SEEK_SET);
    return size;
}

char * readfile(char * filename) {
    FILE *f; f = fopen(filename, "r");
    if (!f) {
        printf("Read file error [%s]", filename);
        exit(0);
    }

    unsigned long size = get_size(f);
    char byte = '\0';
    char * output_stream = (char *)calloc(size + 1, sizeof(char));
    unsigned long output_stream_counter = 0;

    while (byte != EOF) {
        byte = getc(f);
        if (byte != EOF) {
            output_stream[output_stream_counter] = byte;
            output_stream_counter++;
        }
    }
    fclose(f);
    return output_stream;
}

char * cut_structure_node(char * input_stream) {
    char stack_tmp_node[FPARSER_MAX_STRUCT_LEN + 1]; char symbol;
    int node_counter = 0; int quote_counter = 0;

    while (symbol = pop_up(input_stream)) {
        if (symbol == FPARSER_QUOTE) (!quote_counter)  ? quote_counter++ : quote_counter--;
        if (!quote_counter && (symbol == FPARSER_NODE_DELIMITER || symbol == FPARSER_NODE_DELIMITER_2)) break;
        else stack_tmp_node[node_counter++] = symbol;
    }
    stack_tmp_node[node_counter] = '\0';
    char * node = alloc_string(stack_tmp_node);
    return node;
}

char * cut_complex_structure(char * input_stream) {
    char stack_tmp_structure[FPARSER_MAX_STRUCT_LEN + 1]; char symbol;
    int node_counter = 0; pop_up(input_stream); // pop the first '{'

    int line = 1;
    int open_delimiter = 1; int close_delimiter = 0; int quote_counter = 0;
    while (symbol = pop_up(input_stream)) {
        switch (symbol) {
            case FPARSER_QUOTE:
                (!quote_counter) ? quote_counter++ : quote_counter--;
                break;
            case FPARSER_COMPLEX_DELIMITER:
                (quote_counter) ?: open_delimiter++;
                break;
            case FPARSER_COMPLEX_DELIMITER_CLOSE:
                (quote_counter) ?: close_delimiter++;
                break;
            case FPARSER_EOL:
                line++;
                break;
        }
        if (close_delimiter == open_delimiter) break;
        stack_tmp_structure[node_counter++] = symbol;
    }

    stack_tmp_structure[node_counter] = '\0';
    char * complex_structure = alloc_string(stack_tmp_structure);
    return complex_structure;
}

Array ** recursive_descent(char * input_stream) {
    Array ** code_structure;
    if (*input_stream) code_structure = new_array();
    while (*input_stream) {
        if (*input_stream == FPARSER_COMPLEX_DELIMITER) {
            char * nodes = cut_complex_structure(input_stream);
            Array ** structure_nodes = recursive_descent(nodes);
            code_structure = append(code_structure, ARRAY, structure_nodes); free(nodes);
        } else if (!is_ignored(*input_stream)) {
            char * node = cut_structure_node(input_stream);
            if (*node) code_structure = append(code_structure, STRING, node);
        } else pop_up(input_stream);
    }
    return code_structure;
}

/* Environment tools */

char * alloc_string(char stack_tmp_structure[FPARSER_MAX_STRUCT_LEN + 1]) {
    size_t size = 0;
    while (stack_tmp_structure[size]) size++;
    char * structure; structure = (char *)calloc(size + 1, sizeof(char));
    strcpy(structure, stack_tmp_structure);
    return structure;
}

char pop_up(char * input_stream) {
    char symbol;
    if (*input_stream) {
        symbol = *input_stream;
        input_stream++;
        strcpy(input_stream - 1, input_stream);
    } else symbol = '\0';
    return symbol;
}

int is_ignored(char symbol) {
    switch (symbol) {
        case FPARSER_SPACE:
            return 1;
        case FPARSER_EOL:
            return 1;
    }
    return 0;
}