#include "ufo_torrent.h"

#include <iostream>
#include <ncurses.h>
#include <string.h>

#include "../includes/decode.h"
#include "../includes/encode.h"
#include "../includes/prettyPrinter.h"


using namespace bencode;

int main()
{

    string file =
        "d8:announce42:udp://tracker.opentrackr.org:1337/"
        "announce13:announce-listll42:udp://tracker.opentrackr.org:1337/"
        "announceel35:udp://open.tracker.cl:1337/announceel33:udp://"
        "open.stealth.si:80/announceel41:udp://tracker.torrent.eu.org:451/"
        "announceel32:udp://explodie.org:6969/announceee10:created by31:★★★ "
        "megaseed.kz ★★★13:creation "
        "datei1729683364e4:infod5:filesld6:lengthi1470559189e4:pathl8:data."
        "bineed6:lengthi2485956e4:pathl9:data0.bineed6:lengthi63294816e4:"
        "pathl9:Setup.exeeee4:name18:Factorio by Igruha12:piece "
        "lengthi2097152e6:pieces14660:��+������܈�a�R ���Q騆9��~ES���� "
        "������o�]�jў�tB%���ǵ�~���^������`r���v~"
        "�";

    Value torrent = Decoder::decode(file);
    boost::apply_visitor(PrettyPrinter(), torrent);

    //    std::cout << Encoder::encode(10);
}
