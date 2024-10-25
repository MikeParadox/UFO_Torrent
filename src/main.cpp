
#include "ufo_torrent.h"

#include <boost/any.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <iomanip>
#include <iostream>
#include <ncurses.h>
#include <string.h>

int main()
{
    using LongFloat = boost::multiprecision::cpp_bin_float_quad;

    const auto x = boost::multiprecision::int128_t(1234123521);
    const auto y = LongFloat(34532.52346246234);
    const auto z = LongFloat(x) / y;
    std::cout << "Ratio: " << std::setprecision(10) << z << "\n";
}