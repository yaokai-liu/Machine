cmake_minimum_required(VERSION 3.28)
project(machine C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)

add_compile_options("-O3" "-Os")
add_compile_options("-funsigned-char")
add_compile_options("-Wall" "-Wextra" "-Werror")
if (ENABLE_TEST_COV)
    add_compile_options("-fprofile-arcs" "-ftest-coverage")
endif ()

include_directories(meman grammar grammar/generated)

aux_source_directory(meman CTR_SRC)
add_library(meman SHARED ${CTR_SRC})
link_libraries(meman)

aux_source_directory(grammar GRAMMAR_SRC)
aux_source_directory(grammar/generated GRAMMAR_GENERATED_SRC)
add_library(grammar ${GRAMMAR_SRC} ${GRAMMAR_GENERATED_SRC})

aux_source_directory(codegen/C CODEGEN_C_SRC)
add_library(codegen_C ${CODEGEN_C_SRC})
target_include_directories(codegen_C PRIVATE codegen codegen/C)

add_executable(debug test/debug.c)
target_link_libraries(debug PRIVATE grammar codegen_C)
target_include_directories(debug PRIVATE codegen/C)

aux_source_directory(test/common TEST_COMMON_SRC)
aux_source_directory(test/parse TEST_PARSE_SRC)
aux_source_directory(test/tokenize TEST_TOKENIZE_SRC)
add_executable(test-tokenize test/test-tokenize.c ${TEST_COMMON_SRC} ${TEST_TOKENIZE_SRC})
add_executable(test-parse test/test-parse.c ${TEST_COMMON_SRC} ${TEST_PARSE_SRC})
add_executable(test-all test/test-all.c ${TEST_COMMON_SRC} ${TEST_TOKENIZE_SRC} ${TEST_PARSE_SRC})
target_include_directories(test-tokenize PRIVATE test)
target_include_directories(test-parse PRIVATE test)
target_include_directories(test-all PRIVATE test)
target_link_libraries(test-tokenize PRIVATE check grammar)
target_link_libraries(test-parse PRIVATE check grammar)
target_link_libraries(test-all PRIVATE check grammar)
