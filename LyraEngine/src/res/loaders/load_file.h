#pragma once

#include <core/logger.h>
#include <fstream>

namespace lyra {

// mode to open a file
enum OpenMode {
	MODE_INPUT = 0x01,
	MODE_OUTPUT = 0x02,
	MODE_BINARY = 0x20,
	MODE_START_AT_END = 0x04,
	MODE_APPEND = 0x08,
	MODE_TRUNCATE = 0x10
};

/**
 * @brief load a file
 *
 * @param path path
 * @param mode mode to open the file
 * @param file the file
 */
void load_file(const char* path, const int mode, std::ifstream& file);

/**
 * @brief load a file and store all of its contents
 *
 * @tparam data type to store the file
 * @param path path
 * @param mode mode to open the file
 * @param data type to dump the file into
 */
template<class _Ty> void load_file(const char* path, const int mode, _Ty* data) {
	// load the binary
	std::ifstream file;
	load_file(path, mode, file);

	file.seekg(0, file.end);
	size_t size = (size_t)file.tellg();
	file.seekg(0, file.beg);

	data->resize(size);
	file.read(data->data(), size);
	file.close();
}

} // namespace lyra
