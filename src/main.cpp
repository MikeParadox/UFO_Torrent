#include "ufo_torrent.h"

#include <iostream>
#include <ncurses.h>
#include <string>

#include "../includes/prettyPrinter.h"
#include "../includes/decode.h"
#include "../includes/torrentFile.h"
#include "../includes/encode.h"
#include "../includes/fileUtils.h"
#include "../includes/createHash.h"
#include <boost/locale.hpp>

using namespace bencode;
using namespace Torrent;
using namespace File;
using namespace Hash;


int main()
{
    TorrentFile torrent = createTorrentFile(
        "exemple",
        {
            { "exemple" },
            { "aaaaaaa" }
        },
        "MyProject",
        "test",
        "John Doe",
        std::time(nullptr)
    );

    std::string data = Encoder::encode(toValue(torrent));

    // createFile("aa.torrent", data);
}