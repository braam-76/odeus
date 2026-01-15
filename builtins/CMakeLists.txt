add_library(
  builtins STATIC
  constrol_flow.c
  forms.c
  list.c
  math.c
  stdio.c
  typep.c
  strings.c
  set_builtins.c)

target_link_libraries(builtins PUBLIC core)

if(NOT WIN32)
  target_link_libraries(builtins PUBLIC m)
endif()

target_include_directories(
  builtins PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                  $<INSTALL_INTERFACE:include>)
