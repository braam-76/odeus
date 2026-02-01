cmake_minimum_required(VERSION 3.16)
project(OdeusLisp VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic -Wimplicit-fallthrough -Wno-sign-compare)

add_subdirectory(core)
add_subdirectory(builtins)

# REPL executable
add_executable(odeus bin/odeus.c)
target_include_directories(odeus PRIVATE ${PROJECT_SOURCE_DIR}/bin)
target_link_libraries(odeus PRIVATE builtins readline)
