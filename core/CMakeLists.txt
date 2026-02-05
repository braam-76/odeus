add_library(core STATIC
    ast.c
    environment.c
    eval.c
    lexer.c
    parser.c
    quasiquote.c
    value.c
    value_to_string.c
    intern_string.c
  )

target_include_directories(core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
