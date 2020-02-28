#include "exception.h"

void throw_code_structure_exception(int line) {
    printf(EXCEPTION_CODE_STRUCTURE_ERROR, line);
    exit(0);
}

