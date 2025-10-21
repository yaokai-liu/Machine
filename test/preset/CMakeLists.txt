cmake_minimum_required(VERSION 3.28)
project(machine C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")


aux_source_directory(../../meman CTR_SRC)
add_library(meman SHARED ${CTR_SRC})
include_directories(../../meman)

add_compile_options("-O3" "-Os")
add_compile_options("-Wno-unused-parameter" "-funsigned-char" "-fno-builtin")


if (WIN32)
    set(MACHINE_COMPILER "${CMAKE_CURRENT_SOURCE_DIR}/../../output/machine.exe")
else()
    set(MACHINE_COMPILER "${CMAKE_CURRENT_SOURCE_DIR}/../../output/machine")
endif()
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

set(X64_PRESET_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../../preset/x64")
set(X64_PRESET_GENERATED_SRC "${CMAKE_CURRENT_SOURCE_DIR}/x64/x64.c")
if(NOT EXISTS ${X64_PRESET_GENERATED_SRC})
    message(STATUS "preset-x64 source files does not exist or not complete, generate the files.")
    if (NOT EXISTS ${MACHINE_COMPILER})
        message(SEND_ERROR "machine compiler does not exist, stop action.")
    else ()
        execute_process(COMMAND ${EXECUTOR_PATH} ${CMD_HINT}
                "${MACHINE_COMPILER} ${X64_PRESET_SOURCE} x64/x64 -y 2025 -a \"Yaokai Liu\""
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

    endif ()
else()
    message(STATUS "preset-x64 source files exists, skip to generate grammar action table.")
endif()

add_library(preset-x64 x64/x64.c x64/x64.h)
target_link_libraries(preset-x64 PRIVATE meman)

add_executable(preset-debug preset-debug.c )
target_include_directories(preset-debug PRIVATE output)
target_include_directories(preset-debug PRIVATE x64)
target_link_libraries(preset-debug PRIVATE meman preset-x64)

file(GLOB_RECURSE TEST_PRESET_X64_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} x64/test-*.c x64/test-*.h)
add_executable(test-preset-x64 test-preset-x64.c ${TEST_PRESET_X64_SRC})
target_include_directories(test-preset-x64 PRIVATE preset/x64)
target_link_libraries(test-preset-x64 PRIVATE meman preset-x64)
