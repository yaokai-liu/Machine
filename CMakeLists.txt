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

if (WIN32)
    find_program(POWERSHELL_PATH
            NAMES pwsh powershell.exe
            PATHS "C:/Program Files/PowerShell"
            "C:/Windows/System32/WindowsPowerShell")
    set(EXECUTOR_PATH ${POWERSHELL_PATH})
    set(CMD_HINT "-command")
else()
    find_program(SHELL_PATH NAMES bash)
    set(EXECUTOR_PATH ${SHELL_PATH})
    set(CMD_HINT "-c")
endif()
find_package(Python 3.8 REQUIRED)

set(AUTOMATON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/json")
set(GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/grammar/generated")
set(PROJECT_TEMPLATE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/template")
set(AUTOMATON_GENERATOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/liu-machine")
set(SOURCE_GENERATOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/xnf-json2C")

list(APPEND PROJECT_AUTOMATONS "machine" "macro")
list(APPEND AUTOMATON_FILES
        "automaton.json"
        "first_set.json"
        "folded_states.json"
        "follow_set.json"
        "lr_items.json"
        "rules.json"
        "tokens.json")
# create automaton and source directories
foreach(AUTOMATON IN LISTS PROJECT_AUTOMATONS)
    if (NOT EXISTS "${AUTOMATON_DIR}/${AUTOMATON}")
        message(STATUS "${AUTOMATON}'s automaton directory does not exist, make the directory.")
        file(MAKE_DIRECTORY "${AUTOMATON_DIR}/${AUTOMATON}")
    else()
        message(STATUS "${AUTOMATON}'s automaton directory exists, skip.")
    endif ()
    if (NOT EXISTS "${GENERATED_DIR}/${AUTOMATON}")
        message(STATUS "${AUTOMATON}'s automaton source directory does not exist, make the directory.")
        file(MAKE_DIRECTORY "${GENERATED_DIR}/${AUTOMATON}")
    else()
        message(STATUS "${AUTOMATON}'s automaton source directory exists, skip.")
    endif ()
endforeach()
# generate automaton files
execute_process(COMMAND ${Python_EXECUTABLE}
        "${AUTOMATON_GENERATOR_DIR}/generate.py" "${AUTOMATON_DIR}"
        RESULT_VARIABLE PS_RESULT
        OUTPUT_VARIABLE PS_OUTPUT
        ERROR_VARIABLE PS_ERROR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
if (NOT "${PS_OUTPUT}" STREQUAL "")
    message("   ${PS_OUTPUT}")
endif()
if (NOT "${PS_ERROR}" STREQUAL "")
    message(SEND_ERROR "${PS_ERROR}")
endif()
# generate source files
execute_process(COMMAND ${Python_EXECUTABLE}
        "${SOURCE_GENERATOR_DIR}/codegen.py" "${AUTOMATON_DIR}" "${PROJECT_TEMPLATE_DIR}" "${GENERATED_DIR}"
        RESULT_VARIABLE PS_RESULT
        OUTPUT_VARIABLE PS_OUTPUT
        ERROR_VARIABLE PS_ERROR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
if (NOT "${PS_OUTPUT}" STREQUAL "")
    message("   ${PS_OUTPUT}")
endif()
if (NOT "${PS_ERROR}" STREQUAL "")
    message(SEND_ERROR "${PS_ERROR}")
endif()

aux_source_directory(meman CTR_SRC)
add_library(meman ${CTR_SRC})
link_libraries(meman)

include_directories(meman grammar)

file(GLOB_RECURSE GRAMMAR_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} grammar/*.c grammar/*.h)
add_library(grammar ${GRAMMAR_SRC})

aux_source_directory(codegen CODEGEN_SRC)

add_executable(machine compiler/compile.c)
target_link_libraries(machine PRIVATE grammar)
target_include_directories(machine PRIVATE codegen)

if (MACHINE_GENERATE_FORM_ELF)
    aux_source_directory(codegen/elf CODEGEN_ELF_SRC)
    aux_source_directory(codegen/elf/x64 CODEGEN_x86_64_SRC)
    add_library(codegen_elf_x86_64 ${CODEGEN_SRC} ${CODEGEN_ELF_SRC} ${CODEGEN_x86_64_SRC})
    target_include_directories(codegen_elf_x86_64 PRIVATE codegen codegen/elf codegen/elf/x64)
    target_link_libraries(machine PRIVATE codegen_elf_x86_64)
else ()
    aux_source_directory(codegen/C CODEGEN_C_SRC)
    add_library(codegen_C ${CODEGEN_SRC} ${CODEGEN_C_SRC})
    target_include_directories(codegen_C PRIVATE codegen codegen/C)
    target_link_libraries(machine PRIVATE codegen_C)
endif ()

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
