#include "exception.h"

void throw_code_structure_exception(int line, char * message) {
    printf(EXCEPTION_CODE_STRUCTURE_ERROR, line, message);
    exit(0);
}

