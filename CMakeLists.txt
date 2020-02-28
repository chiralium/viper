cmake_minimum_required(VERSION 3.15)
project(viper_v4 C)

set(CMAKE_C_STANDARD 99)

add_executable(viper_v4 main.c run.h run.c fread.h fread.c array.h array.c types.h exception.h exception.c display.h display.c)