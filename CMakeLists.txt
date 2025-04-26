cmake_minimum_required(VERSION 3.19)

project("ufo_torrent"
        VERSION 0.1
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


#option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)."
#        TRUE)
#if (${FORCE_COLORED_OUTPUT})
#    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
#        add_compile_options(-fdiagnostics-color=always)
#    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#        add_compile_options(-fcolor-diagnostics)
#   endif ()
#endif ()


enable_testing()

include_directories(includes)
include_directories(includes/network_part)
include_directories(libs/crypto)
include_directories(libs/bencode)
add_subdirectory("src")
#add_subdirectory("tests")

target_compile_options(main PRIVATE
        -g -ggdb -Werror -Wall -Wpedantic -Wno-parentheses)