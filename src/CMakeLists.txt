# CMakeList.txt: проект CMake для ufo_torrent; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.

find_package(Curses REQUIRED)
include_directories(${CURSES_INDLUDE_DIRS})

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS}) 

add_library(Testing ../includes/ufo_torrent.h ufo_torrent.cpp ./models/torrentFile.cpp
                      ./bencode/decode.cpp
                      ./bencode/prettyPrinter.cpp
                      ./bencode/encode.cpp
                      ./bencode/prettyPrinter.cpp "ncurses_utils.cpp")


add_executable(${PROJECT_NAME} main.cpp)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.45.0 COMPONENTS *boost libraries here*) 

# if(Boost_FOUND)
#     add_executable(progname file1.cxx file2.cxx) 
#     target_link_libraries(progname ${Boost_LIBRARIES})
# endif()


target_link_libraries(${PROJECT_NAME} PRIVATE 
  Testing
  ${CURSES_LIBRARIES}
  ${Boost_LIBRARIES}
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ufo_torrent PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.


