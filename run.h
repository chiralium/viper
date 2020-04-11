#include "fread.h"
#include "lexer.h"
#include "parser.h"
#include "expression.h"
#include "arithmetica.h"
#include "interpreter.h"
#include "display.h"

#ifndef VIPER_V4_RUN_H
#define VIPER_V4_RUN_H

Array ** call_stack;

// Main entry point
void run(void);

#endif //VIPER_V4_RUN_H
