find_package(Curses REQUIRED)
include_directories(${CURSES_INDLUDE_DIRS})

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# find_package(cpr REQUIRED)
# include_directories ( ${CPR_INCLUDE_DIRS} )

add_library(Testing ../includes/ufo_torrent.h ufo_torrent.cpp 
                      ./bencode/decode.cpp
                      ./bencode/prettyPrinter.cpp
                      ./bencode/encode.cpp
                      ./network_part/BitTorrentMessage.cpp
                      ./network_part/connect.cpp
                      ./network_part/PeerConnection.cpp
                      ./network_part/PeerRetriever.cpp
                      ./network_part/Piece.cpp
                      ./network_part/PieceManager.cpp
                      ./network_part/TorrentClient.cpp
                      ./network_part/TorrentFileParser.cpp
                      ./network_part/utils.cpp)

add_executable(main main.cpp)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.45.0 COMPONENTS *boost libraries here*) 

target_link_libraries(main PRIVATE 
  Testing
  ${CURSES_LIBRARIES}
  ${Boost_LIBRARIES}
  # cpr::cpr
)

