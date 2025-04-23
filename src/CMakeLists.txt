find_package(LibtorrentRasterbar REQUIRED)
include_directories(${LibtorrentRasterbar_INCLUDE_DIRS})

find_package(Curses REQUIRED)
include_directories(${CURSES_INDLUDE_DIRS})

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})


add_library(Testing STATIC ../includes/ufo_torrent.h ufo_torrent.cpp
        ./bencode/decode.cpp
        ./bencode/prettyPrinter.cpp
        ./bencode/encode.cpp
        ./hashing/createHash.cpp
        ./utils/fileUtils.cpp
        ./models/torrentFile.cpp
        ./network_part/BitTorrentMessage.cpp
        ./network_part/connect.cpp
        ./network_part/PeerConnection.cpp
        ./network_part/PeerRetriever.cpp
        ./network_part/Piece.cpp
        ./network_part/PieceManager.cpp
        ./network_part/TorrentClient.cpp
        ./network_part/TorrentFileParser.cpp
        ./network_part/utils.cpp
        ../libs/bencode/Utils.cpp
        ../libs/bencode/Decoder.cpp
        ../libs/bencode/Encoder.cpp
        ../libs/bencode/BItemVisitor.cpp
        ../libs/bencode/BItem.cpp
        ../libs/bencode/BInteger.cpp
        ../libs/bencode/Decoder.cpp
        ../libs/bencode/BString.cpp
        ../libs/bencode/BDictionary.cpp
        ../libs/bencode/BList.cpp
        ../libs/crypto/sha1.cpp
)

add_executable(main main.cpp)

# For fully static linking, we need to link against static versions of system libraries
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

# If you want completely static (including libc), use:
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

#find_package(Boost 1.45.0 COMPONENTS system filesystem)

target_link_libraries(main PRIVATE
        Testing
        ${CURSES_LIBRARIES}
        ${Boost_LIBRARIES}
        ${CURL_LIBRARIES}
        #        LibtorrentRasterbar::torrent-rasterbar
)