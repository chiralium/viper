#include <stdio.h>
#include "array.h"
#include "lexer.h"
#include "parser.h"
#include "expression.h"

/*
 * The tools for formatted display a complex structure
 */

#ifndef VIPER_V4_DISPLAY_H
#define VIPER_V4_DISPLAY_H

void display_array(Array ** _array);
void display_statements(void * statement, char type_id);

#endif //VIPER_V4_DISPLAY_H
