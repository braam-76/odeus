cmake_minimum_required(VERSION 3.16)
project(OdeusLisp VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic -Wimplicit-fallthrough -Wno-sign-compare)

include_directories(core tests)

# Create a static library that combines all core components
add_library(odeus_core STATIC
    core/odeus_lexer.c
    core/odeus_parser.c
)

# Lexer test executable
add_executable(lexer_test tests/odeus_lexer_test.c)
target_link_libraries(lexer_test PRIVATE odeus_core)

# Parser test executable
add_executable(parser_test tests/odeus_parser_test.c)
target_link_libraries(parser_test PRIVATE odeus_core)

enable_testing()
add_test(NAME LexerTest COMMAND lexer_test ${CMAKE_CURRENT_SOURCE_DIR}/tests/odeus_lexer_test.ode)
add_test(NAME ParserTest COMMAND parser_test ${CMAKE_CURRENT_SOURCE_DIR}/tests/odeus_parser_test.ode)
