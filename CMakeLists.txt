cmake_minimum_required(VERSION 3.16)
project(OdeusLisp VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic -Wimplicit-fallthrough -Wno-sign-compare)


# Find pkg-config
find_package(PkgConfig REQUIRED)

# Find Boehm GC using pkg-config
pkg_check_modules(GC REQUIRED bdw-gc)

# Debug message to see what was found
message(STATUS "GC include dirs: ${GC_INCLUDE_DIRS}")
message(STATUS "GC libraries: ${GC_LIBRARIES}")

add_subdirectory(core)
add_subdirectory(builtins)

# REPL executable
add_executable(odeus bin/odeus.c)

target_include_directories(odeus PRIVATE ${PROJECT_SOURCE_DIR}/bin)
target_include_directories(odeus PRIVATE ${GC_INCLUDE_DIRS})
target_link_libraries(odeus PRIVATE builtins readline ${GC_LIBRARIES})
