add_library(builtins STATIC
  constrol_flow.c
  forms.c
  list.c
  math.c
  stdio.c
  typep.c

  set_builtins.c
)

target_link_libraries(builtins PUBLIC core)

target_include_directories(builtins PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
