cmake_minimum_required(VERSION 3.15)
project(viper_v4 C)

set(CMAKE_C_STANDARD 99)

add_executable(viper_v4 main.c run.h run.c fread.h fread.c array.h array.c types.h exception.h exception.c display.h display.c lexer.h operators.h lexer.c keywords.h parser.h parser.c expression.h expression.c arithmetica.h arithmetica.c interpreter.h interpreter.c namespace.h namespace.c ViArray.c functions.h functions.c composer.h composer.c ViString.c ViString.h memory.c memory.h types.c builtins.h builtins.c)

target_link_libraries(viper_v4 m)