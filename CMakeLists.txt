cmake_minimum_required(VERSION 3.16)
project(OdeusLisp VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic -Wimplicit-fallthrough -Wno-sign-compare)

# Core and test includes
include_directories(
  ${PROJECT_SOURCE_DIR}/core
  ${PROJECT_SOURCE_DIR}/tests
  /usr/include
)

link_directories(
  /usr/lib
)

add_library(odeus_core STATIC
    core/odeus_lexer.c
    core/odeus_parser.c
    core/builtins/odeus_basic_math.c
    core/builtins/odeus_primitives.c
    core/builtins/forms.c
    core/builtins/set_builtins.c
    core/environment.c
    core/eval.c
    core/ast.c
)

# REPL executable with readline
add_executable(repl repl/repl.c)
target_link_libraries(repl PRIVATE core readline)

# Lexer test executable
add_executable(lexer_test tests/lexer_test.c)
target_link_libraries(lexer_test PRIVATE core readline)

# Parser test executable
add_executable(parser_test tests/parser_test.c)
target_link_libraries(parser_test PRIVATE core readline)

enable_testing()
add_test(NAME LexerTest COMMAND lexer_test ${CMAKE_CURRENT_SOURCE_DIR}/tests/lexer_test.ode)
add_test(NAME ParserTest COMMAND parser_test ${CMAKE_CURRENT_SOURCE_DIR}/tests/parser_test.ode)
