cmake_minimum_required(VERSION 3.16)
project(OdeusLisp VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic -Wimplicit-fallthrough -Wno-sign-compare)

add_subdirectory(core)
add_subdirectory(builtins)

# REPL executable
add_executable(repl repl/repl.c)
target_include_directories(repl PRIVATE ${PROJECT_SOURCE_DIR}/repl)
target_link_libraries(repl PRIVATE builtins readline)

# Lexer test executable
add_executable(lexer_test tests/lexer_test.c)
target_include_directories(lexer_test PRIVATE ${PROJECT_SOURCE_DIR}/tests)
target_link_libraries(lexer_test PRIVATE builtins readline)

# Parser test executable
add_executable(parser_test tests/parser_test.c)
target_include_directories(parser_test PRIVATE ${PROJECT_SOURCE_DIR}/tests)
target_link_libraries(parser_test PRIVATE builtins readline)

enable_testing()
add_test(NAME LexerTest COMMAND lexer_test ${CMAKE_CURRENT_SOURCE_DIR}/tests/lexer_test.ode)
add_test(NAME ParserTest COMMAND parser_test ${CMAKE_CURRENT_SOURCE_DIR}/tests/parser_test.ode)
