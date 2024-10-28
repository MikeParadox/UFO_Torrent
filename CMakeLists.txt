# CMakeList.txt: файл проекта CMake верхнего уровня; проведите глобальную конфигурацию
# и включите сюда подпроекты.
#
cmake_minimum_required (VERSION 3.8)

project ("ufo_torrent"
        VERSION 0.1
        LANGUAGES CXX)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR)
set(CMAKE_GENERATOR "Ninja")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
message("generator is set to ${CMAKE_GENERATOR}")


# Включите подпроекты.
include_directories(includes)
add_subdirectory ("src")
add_subdirectory("tests")

target_compile_options(${PROJECT_NAME} PRIVATE -std=c++2b -fopenmp
-g -ggdb 
-Werror -Wall -Wpedantic -Wno-parentheses)
