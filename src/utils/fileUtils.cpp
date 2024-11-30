#include "fileUtils.h"


unsigned long long File::getFileSize(const std::string& filePath) {
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
	if (!file.is_open())
		throw std::runtime_error("Не удалось открыть файл: " + filePath);

	return file.tellg(); 
}

void File::createFile(const std::string& filePath, const std::string& binaryContent) {
	std::ofstream outFile(filePath, std::ios::binary);

	if (!outFile)
		throw std::runtime_error("Failed to open file: " + filePath);

	outFile.write(binaryContent.c_str(), binaryContent.size());

	if (!outFile)
		throw std::runtime_error("Failed to write data to file: " + filePath);

	outFile.close();
}

string File::read(const std::string& filePath) {
	std::ifstream inputFile(filePath);
	if (!inputFile.is_open())
		throw std::runtime_error("Failed to open file: " + filePath);


	std::string content((std::istreambuf_iterator<char>(inputFile)),
		std::istreambuf_iterator<char>());

	inputFile.close();


	return content;
}
