cmake_minimum_required (VERSION 3.8)

project ("ufo_torrent"
        VERSION 0.1
        LANGUAGES CXX)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")                
# set(CMAKE_CXX_COMPILER "/usr/bin/g++-14")

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)."
 TRUE)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()



include_directories(includes)
include_directories(includes/network_part)
include_directories(libs/crypto)
include_directories(libs/bencode)
add_subdirectory ("src")
add_subdirectory("tests")

target_compile_options(main PRIVATE 
-g -ggdb -Werror -Wall -Wpedantic -Wno-parentheses)