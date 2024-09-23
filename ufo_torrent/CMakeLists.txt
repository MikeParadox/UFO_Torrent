add_library(Testing ../includes/ufo_torrent.h ufo_torrent.cpp)

add_executable(${PROJECT_NAME} main.cpp)
    
target_link_libraries(${PROJECT_NAME} PRIVATE Testing)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ufo_torrent PROPERTY CXX_STANDARD 20)
endif()

