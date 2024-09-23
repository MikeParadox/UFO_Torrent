# CMakeList.txt: проект CMake для ufo_torrent; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
    
add_library(Testing ../includes/ufo_torrent.h ufo_torrent.cpp)

add_executable(${PROJECT_NAME} main.cpp)
    
target_link_libraries(${PROJECT_NAME} PRIVATE Testing)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ufo_torrent PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
