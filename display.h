#include <stdio.h>
#include "array.h"
#include "lexer.h"
#include "parser.h"
#include "expression.h"
#include "arithmetica.h"

/*
 * The tools for formatted display a complex structure
 */

#ifndef VIPER_V4_DISPLAY_H
#define VIPER_V4_DISPLAY_H

void display_array(Array ** _array);
void display_array_beauty(Array ** _array, char tabs[512]);
void display_statements(void * statement, char type_id, char tabs[512]);
void display_index(Index * index, char tabs[512]);
void display_function(FuncCall * function, char tabs[512]);
void display_constant(Constant * constant);

#endif //VIPER_V4_DISPLAY_H
