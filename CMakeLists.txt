cmake_minimum_required(VERSION 3.28)
project(machine C)

set(CMAKE_C_STANDARD 23)

add_compile_options("-O3" "-Os")
add_compile_options("-funsigned-char")
add_compile_options("-Wall" "-Wextra" "-Werror")
if (ENABLE_TEST_COV)
    add_compile_options("-fprofile-arcs" "-ftest-coverage")
endif ()

include_directories(container grammar)

aux_source_directory(container CTR_SRC)
add_library(container ${CTR_SRC})

aux_source_directory(grammar GRAMMAR_SRC)
add_library(grammar ${GRAMMAR_SRC})
target_link_libraries(grammar PRIVATE container)
