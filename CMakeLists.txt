cmake_minimum_required(VERSION 3.16)
project(OdeusLisp VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(core test)

add_library(odeus_lexer OBJECT core/odeus_lexer.c)

add_executable(lexer_test tests/odeus_lexer_test.c)
target_link_libraries(lexer_test PRIVATE $<TARGET_OBJECTS:odeus_lexer>)

enable_testing()
add_test(NAME LexerTest COMMAND lexer_test ${CMAKE_CURRENT_SOURCE_DIR}/tests/odeus_lexer_test.ode)
