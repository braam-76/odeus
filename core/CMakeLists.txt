add_library(core STATIC
    lexer.c
    parser.c
    environment.c
    eval.c
    ast.c
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
