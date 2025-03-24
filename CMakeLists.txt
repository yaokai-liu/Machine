cmake_minimum_required(VERSION 3.28)
project(machine C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

add_compile_options("-O3" "-Os")
add_compile_options("-funsigned-char" "-fno-builtin")
add_compile_options("-Wall" "-Wextra" "-Werror")
if (WIN32)
    add_compile_options("-Wno-format")
endif ()
if (ENABLE_TEST_COV)
    add_compile_options("-fprofile-arcs" "-ftest-coverage")
endif ()
if (ENABLE_GPROF)
    add_compile_options("-pg")
    add_link_options("-pg")
endif ()

set(GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/grammar/generated")

if(NOT EXISTS ${GENERATED_DIR})
    message(STATUS "${GENERATED_DIR} doesn't exist, generate grammar action table.")
    execute_process(COMMAND /bin/sh ${CMAKE_CURRENT_SOURCE_DIR}/codegen.sh
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
else()
    message(STATUS "${GENERATED_DIR} exists, skip to generate grammar action table.")
endif()

aux_source_directory(meman CTR_SRC)
add_library(meman ${CTR_SRC})
link_libraries(meman)

include_directories(meman grammar)

file(GLOB_RECURSE GRAMMAR_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} grammar/*.c grammar/*.h)
add_library(grammar ${GRAMMAR_SRC})

aux_source_directory(codegen CODEGEN_SRC)

aux_source_directory(codegen/C CODEGEN_C_SRC)
add_library(codegen_C ${CODEGEN_SRC} ${CODEGEN_C_SRC})
target_include_directories(codegen_C PRIVATE codegen codegen/C)

aux_source_directory(codegen/elf CODEGEN_ELF_SRC)

aux_source_directory(codegen/elf/x64 CODEGEN_x86_64_SRC)
add_library(codegen_x86_64 ${CODEGEN_SRC} ${CODEGEN_ELF_SRC} ${CODEGEN_x86_64_SRC})
target_include_directories(codegen_x86_64 PRIVATE codegen codegen/elf codegen/elf/x64)

add_executable(machine-c compiler/compile.c)
target_link_libraries(machine-c PRIVATE grammar codegen_C)
target_include_directories(machine-c PRIVATE codegen codegen/C)

add_executable(machine-elf compiler/compile.c)
target_link_libraries(machine-elf PRIVATE grammar codegen_elf)
target_include_directories(machine-elf PRIVATE codegen codegen/elf)

add_executable(debug test/debug.c)
target_link_libraries(debug PRIVATE grammar)

aux_source_directory(test/common TEST_COMMON_SRC)
#aux_source_directory(test/parse TEST_PARSE_SRC)
aux_source_directory(test/tokenize TEST_TOKENIZE_SRC)
add_executable(test-tokenize test/test-tokenize.c ${TEST_COMMON_SRC} ${TEST_TOKENIZE_SRC})
#add_executable(test-parse test/test-parse.c ${TEST_COMMON_SRC} ${TEST_PARSE_SRC})
#add_executable(test-all test/test-all.c ${TEST_COMMON_SRC} ${TEST_TOKENIZE_SRC} ${TEST_PARSE_SRC})
target_include_directories(test-tokenize PRIVATE test)
#target_include_directories(test-parse PRIVATE test)
#target_include_directories(test-all PRIVATE test)
target_link_libraries(test-tokenize PRIVATE check grammar)
#target_link_libraries(test-parse PRIVATE check grammar)
#target_link_libraries(test-all PRIVATE check grammar)
