#include "fileUtils.h"

string File::read(const std::string& filePath) {
	// TODO
	return "";
}

unsigned long long File::getFileSize(const std::string& filePath) {
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
	if (!file.is_open())
		throw std::runtime_error("Не удалось открыть файл: " + filePath);

	return file.tellg(); 
}

void File::createFile(const std::string& filePath, const std::string& binaryContent) {
    // TODO
}
