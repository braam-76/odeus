add_library(builtins STATIC
    basic_math.c
    primitives.c
    forms.c
    set_builtins.c
)

target_link_libraries(builtins PUBLIC core)

target_include_directories(builtins PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
