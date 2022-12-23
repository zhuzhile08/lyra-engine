#pragma once

#include <vector>
#include <fstream>
#include <sstream>
#include <limits> // should have seen the look on my face then I found out that I would have to include another goofy ahh standard libary JUST to count the file size

namespace lyra {

namespace util {

// mode to open a file
#ifdef _WIN32 // try to imagine how long I took to find out that the values were differnt om macOS
enum OpenMode {
	MODE_INPUT = 0x01,
	MODE_OUTPUT = 0x02,
	MODE_BINARY = 0x20,
	MODE_START_AT_END = 0x04,
	MODE_APPEND = 0x08,
	MODE_TRUNCATE = 0x10
};
#elif __APPLE__
enum OpenMode {
	MODE_APPEND = 0x01,
	MODE_START_AT_END = 0x02,
	MODE_BINARY = 0x04,
	MODE_INPUT = 0x08,
	MODE_OUTPUT = 0x10,
	MODE_TRUNCATE = 0x20
};
#elif linux
	std::filesystem::path path("/proc/self/exe");
	std::filesystem::canonical(path);
	return path;
#endif

/**
 * @brief load a file
 *
 * @param path path
 * @param mode mode to open the file
 * @param file the file
 */
void load_file(const char* path, const int& mode, std::ifstream& file);

/**
 * @brief load a file and store all of its contents into a string-like container
 *
 * @tparam Ty type of container
 * @param path path
 * @param mode mode to open the file
 * @param fileContainer container to copy the file into
 */
template<class Ty>
void load_file(const char* path, const int& mode, Ty& fileContainer) {
	// load the binary
	std::ifstream file;
	load_file(path, mode, file);

	// read the value into a stringstream
	std::stringstream buffer;
	buffer << file.rdbuf();
	// move the stringstream into the string
	fileContainer = std::move(buffer.str());

	// close the file
	file.close();
}

/**
 * @brief load a file and store all of its contents into a vector
 * 
 * @tparam Ty type to store in the container
 * @param path path
 * @param mode mode to open the file
 * @param fileContainer container to copy the file into
 */
template<class Ty> 
void load_file(const char* path, const int& mode, std::vector<Ty>& fileContainer) {
	// load the binary
	std::ifstream file;
	load_file(path, mode, file);

	// read the value into a single buffer and push it into the container
	Ty buffer;
	while(file.get(buffer)) fileContainer.push_back(std::move(buffer));

	// close the file
	file.close();
}

} // namespace util

} // namespace lyra
