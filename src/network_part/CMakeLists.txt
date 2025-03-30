# CMakeList.txt: проект CMake для ufo_torrent; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.

find_package(Curses REQUIRED)
include_directories(${CURSES_INDLUDE_DIRS})

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS}) 

add_library(Testing ../includes/ufo_torrent.h ufo_torrent.cpp 
                      ./bencode/decode.cpp
                      ./bencode/prettyPrinter.cpp
                      ./bencode/encode.cpp
                      ./hashing/createHash.cpp
                      ./utils/fileUtils.cpp
                      ./models/torrentFile.cpp)

add_executable(main main.cpp)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.45.0 COMPONENTS *boost libraries here*) 

target_link_libraries(main PRIVATE 
  Testing
  ${CURSES_LIBRARIES}
  ${Boost_LIBRARIES}
)

