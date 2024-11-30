#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

using std::string;
using std::vector;

namespace File {

	string read(const std::string& filePath);

	unsigned long long getFileSize(const std::string& filename);

	void createFile(const std::string& filePath, const std::string& content);
}

#endif