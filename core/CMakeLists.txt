add_library(core STATIC
    lexer.c
    parser.c
    environment.c
    eval.c
    value.c
    value_to_string.c
    quasiquote.c
)

target_include_directories(core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
