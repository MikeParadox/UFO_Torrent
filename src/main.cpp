#include "ufo_torrent.h"

#include <iostream>
#include <ncurses.h>
#include <string>
#include <fstream>

#include "../includes/prettyPrinter.h"
#include "../includes/decode.h"
#include "../includes/torrentFile.h"
#include "../includes/encode.h"
#include <filesystem>

using namespace bencode;
using namespace Torrent;

int main()
{
    TorrentFile file = parseTorrentFile(Decoder::decode(readFile("exemple.torrent")));
    Value valueFile = toValue(file);

    boost::apply_visitor(PrettyPrinter(), valueFile);
}