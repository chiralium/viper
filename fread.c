#include "fread.h"

int ROW_NUMBER = 1; // the global constant which display the current line for exception causes

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
    int node_counter = 0; int _quote_counter = 0; int _obracket_counter = 0; int _cbracket_counter = 0;

    while (symbol = pop_first(input_stream)) {
        switch (symbol) {
            case FPARSER_QUOTE:
                _quote_counter++;
                break;
            case FPARSER_COMPLEX_DELIMITER:
                (!(_quote_counter % 2)) ? _obracket_counter++ : _obracket_counter;
                break;
            case FPARSER_COMPLEX_DELIMITER_CLOSE:
                (!(_quote_counter % 2)) ? _cbracket_counter++ : _cbracket_counter;
                break;
        }

        if ((!(_quote_counter % 2)) && (symbol == FPARSER_NODE_DELIMITER || symbol == FPARSER_NODE_DELIMITER_2)) break;
        else stack_tmp_node[node_counter++] = symbol;
    }

    if (_quote_counter % 2) throw_code_structure_exception(ROW_NUMBER, FPARSER_QUOTE_BALANCED_MSG);
    if ((_obracket_counter - _cbracket_counter) != 0) throw_code_structure_exception(ROW_NUMBER, FPRASE_STRUCT_BALANCED_MSG);

    stack_tmp_node[node_counter] = '\0';
    char * node = alloc_string(stack_tmp_node);
    return node;
}

char * cut_complex_structure(char * input_stream) {
    char stack_tmp_structure[FPARSER_MAX_STRUCT_LEN + 1]; char symbol;
    int node_counter = 0; pop_first(input_stream); // pop the first '{'

    int open_delimiter = 1; int close_delimiter = 0; int _quote_counter = 0;
    while (symbol = pop_first(input_stream)) {
        switch (symbol) {
            case FPARSER_QUOTE:
                _quote_counter++;
                break;
            case FPARSER_COMPLEX_DELIMITER:
                !(_quote_counter % 2) ? open_delimiter++ : open_delimiter;
                break;
            case FPARSER_COMPLEX_DELIMITER_CLOSE:
                !(_quote_counter % 2) ? close_delimiter++ : close_delimiter;
                break;
        }
        if (close_delimiter == open_delimiter) break;
        stack_tmp_structure[node_counter++] = symbol;
    }

    if (close_delimiter - open_delimiter != 0) throw_code_structure_exception(ROW_NUMBER, FPRASE_STRUCT_BALANCED_MSG);
    if (!node_counter) throw_code_structure_exception(ROW_NUMBER, FPRASE_STRUCT_IS_EMPTY_MSG);

    stack_tmp_structure[node_counter] = '\0';
    char * complex_structure = alloc_string(stack_tmp_structure);
    return complex_structure;
}

Array ** recursive_descent(char * input_stream) {
    Array ** code_structure;
    if (*input_stream) code_structure = new_array();
    while (*input_stream) {
        if (*input_stream == FPARSER_EOL) ROW_NUMBER++;
        if (*input_stream == FPARSER_COMPLEX_DELIMITER) {
            char * nodes = cut_complex_structure(input_stream);
            Array ** structure_nodes = recursive_descent(nodes);
            code_structure = append(code_structure, ARRAY, structure_nodes); free(nodes);
        } else if (!is_ignored(*input_stream)) {
            char * node = cut_structure_node(input_stream);
            if (*node) code_structure = append(code_structure, STRING, node);
            else free(node);
        } else pop_first(input_stream);
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

char pop_first(char * input_stream) {
    char symbol;
    if (*input_stream) {
        symbol = *input_stream;
        input_stream++;
        strcpy(input_stream - 1, input_stream);
    } else symbol = '\0';
    return symbol;
}

char pop_last(char * input_stream) {
    char symbol;
    if (*input_stream) {
        int length = strlen(input_stream) - 1;
        symbol = input_stream[length];
        input_stream[length] = '\0';
        return symbol;
    } else {
        return 0;
    }
}

int is_ignored(char symbol) {
    switch (symbol) {
        case FPARSER_SPACE:
        case FPARSER_EOL:
            return 1;
    }
    return 0;
}