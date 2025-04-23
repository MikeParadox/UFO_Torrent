/**
 * @file fileUtils.h
 * @brief File system utilities.
 * @details Provides basic file operations for torrent client.
 */
#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

using std::string;
using std::vector;

namespace File {

    /**
     * @brief Reads entire file contents.
     * @param filePath Path to file.
     * @return File contents as string.
     */
    string read(const std::string& filePath);

    /**
     * @brief Gets file size.
     * @param filename Path to file.
     * @return File size in bytes.
     */
    unsigned long long getFileSize(const std::string& filename);

    /**
     * @brief Creates new file with content.
     * @param filePath Path to new file.
     * @param content File contents.
     */
    void createFile(const std::string& filePath, const std::string& content);
}

#endif