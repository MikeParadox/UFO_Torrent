/**
 * @file Utils.h
 * @brief Utility functions for bencoding library.
 * @details Provides helper functions for string/number conversion and stream operations.
 */
#ifndef BENCODING_UTILS_H
#define BENCODING_UTILS_H

#include <ios>
#include <istream>
#include <queue>
#include <sstream>
#include <stack>
#include <string>

namespace bencoding {

    /// @name String/Number Conversion
    /// @{
    /**
     * @brief Converts string to number
     * @tparam N Target numeric type
     * @param str Input string
     * @param num Output variable
     * @param format Number format (e.g. std::dec)
     * @return true if conversion succeeded
     */
    template<typename N>
    bool strToNum(const std::string& str, N& num,
        std::ios_base& (*format)(std::ios_base&) = std::dec) {
        std::istringstream stream(str);
        N convNum = 0;
        stream >> format >> convNum;
        return !stream.fail() && stream.eof();
    }
    /// @}

    /// @name Stream Operations
    /// @{
    bool readUpTo(std::istream& stream, std::string& readData, char sentinel);
    bool readUntil(std::istream& stream, std::string& readData, char last);
    /// @}

    /// @name String Operations
    /// @{
    std::string replace(const std::string& str, char what,
        const std::string& withWhat);
    /// @}

} // namespace bencoding
#endif