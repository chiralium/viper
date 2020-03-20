#include "interpreter.h"

void interpreter(Array ** code) {
    Node * root = meta_data(); // init.  the namespace by insert some meta-info into it
    int code_counter = 0;
    while (code[code_counter]) {
        if (code[code_counter]->type_id == ARRAY) {
            // if this condition is true, then this element is a expression
            Constant * value = calculate_expression(code[code_counter]->element, root);
            display_constant(value);
            constant_destructor(value);
            free(code[code_counter]);
        } else if (code[code_counter]->type_id == STMT_IF) {
            if_destructor(code[code_counter]->element);
            free(code[code_counter]);
        } else if (code[code_counter]->type_id == STMT_WHILE) {
            free(code[code_counter]->element);
            free(code[code_counter]);
        } else if (code[code_counter]->type_id == STMT_FUNC) {
            function_destructor(code[code_counter]->element);
            free(code[code_counter]);
        }
        code_counter++;
    }
    free(code);
    namespace_destructor(root);
}

Constant * calculate_expression(Array ** expression, Node * current_namespace) {
    Constant * value = arithmetica(expression, current_namespace);
    return value;
}

Node * meta_data() {
    char * ver = calloc(sizeof(char), 10); strcpy(ver, "VIPER.v4");
    Constant * version = new_constant(STRING, ver);

    float * pi = malloc(sizeof(float)); *pi = 3.14;
    Constant * pi_number = new_constant(FLOAT, pi);

    Node * root = new_node(faq6("pi"), pi_number);
    Node * ver_node = new_node(faq6("version"), version);

    insert_node(root, ver_node);

    return root;
}