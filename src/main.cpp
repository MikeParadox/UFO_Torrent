#include "ufo_torrent.h"

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <iomanip>
#include <iostream>
#include <ncurses.h>
#include <string.h>
#include "./bencode/decode.h"
#include "bencode/prettyPrinter.h"

using namespace bencode;

int main()
{
    using LongFloat = boost::multiprecision::cpp_bin_float_quad;

    const auto x = boost::multiprecision::int128_t(1234123521);
    const auto y = LongFloat(34532.52346246234);
    const auto z = LongFloat(x) / y;
    std::cout << "Ratio: " << std::setprecision(10) << z << "\n";
 
    std::string s = "i10e";

    Decoder::decode(s);

    Value torrent = boost::get<ValueDictionary>(Decoder::decode("d8:announce42:udp://tracker.opentrackr.org:1337/announce13:announce-listll42:udp://tracker.opentrackr.org:1337/announceel35:udp://open.tracker.cl:1337/announceel33:udp://open.stealth.si:80/announceel41:udp://tracker.torrent.eu.org:451/announceel32:udp://explodie.org:6969/announceee10:created by31:â˜…â˜…â˜… megaseed.kz â˜…â˜…â˜…13:creation datei1729683364e4:infod5:filesld6:lengthi1470559189e4:pathl8:data.bineed6:lengthi2485956e4:pathl9:data0.bineed6:lengthi63294816e4:pathl9:Setup.exeeee4:name18:Factorio by Igruha12:piece lengthi2097152e6:pieces14660:©€+ş‰øì÷¹Üˆ¢a‡R öãÀQé¨†9˜•~ESñæí© ¹•³÷¤oç¾]ájÑ÷tB%ÊÉÇµ…~éó ^÷Èí¶­œ`rµ­›v~ş"));
    boost::apply_visitor(PrettyPrinter(), torrent);
}