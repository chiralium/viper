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

#define COLOR_SCHEME_WARNING            0
#define COLOR_SCHEME_EXCEPTION          1
#define COLOR_SCHEME_CALL_STACK_TAIL    3
#define COLOR_SCHEME_CALL_STACK_HEAD    4
#define COLOR_SCHEME_INTERPRETER_OUTPUT 5
#define COLOR_SCHEME_MEMORY_TABLE       6
#define COLOR_SCHEME_DEFAULT            7

void display_array(Array ** _array);
void display_array_beauty(Array ** _array, char tabs[512]);
void display_statements(void * statement, char type_id, char tabs[512]);
void display_index(Index * index, char tabs[512]);
void display_function(FuncCall * function, char tabs[512]);
void display_constant(Constant * constant);
void display_callstack(Array ** call_stack);
void set_color_scheme(int scheme);

#endif //VIPER_V4_DISPLAY_H
