#include "types.h"
#include <string.h>

char * get_type_label(char type_id) {
    static char label[100];
    switch (type_id) {
        case INTEGER: strcpy(label, "integer"); break;
        case FLOAT: strcpy(label, "float"); break;
        case STRING: strcpy(label, "string"); break;
        case VIARRAY: strcpy(label, "viarray"); break;
        case KEYPAIR: strcpy(label, "keypair"); break;
        case NAMESPACE: strcpy(label, "namespace"); break;
        case BUILT_IN_FUNCTION: strcpy(label, "built-in"); break;
        case NONE: strcpy(label, "none"); break;
        case FUNCTION: case FUNCTION_CONTAINER: strcpy(label, "function"); break;
        default: strcpy(label, "undefined");
    }
    return label;
}