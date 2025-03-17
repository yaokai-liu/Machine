cmake_minimum_required(VERSION 3.28)
project(machine C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

#
#aux_source_directory(../../meman CTR_SRC)
#add_library(meman SHARED ${CTR_SRC})
#include_directories(../../meman)

add_compile_options("-Wno-unused-parameter" "-funsigned-char" "-fno-builtin")

add_library(preset-x64 x64/x64.c x64/x64.h)
target_link_libraries(preset-x64 PRIVATE meman)

add_executable(preset-debug preset-debug.c )
target_include_directories(preset-debug PRIVATE output)
target_link_libraries(preset-debug PRIVATE meman preset-x64)
target_include_directories(preset-debug PRIVATE .)

file(GLOB_RECURSE TEST_PRESET_X64_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} x64/test-*.c x64/test-*.h)
add_executable(test-preset-x64 test-preset-x64.c ${TEST_PRESET_X64_SRC})
target_include_directories(test-preset-x64 PRIVATE preset/x64)
target_link_libraries(test-preset-x64 PRIVATE meman preset-x64)
