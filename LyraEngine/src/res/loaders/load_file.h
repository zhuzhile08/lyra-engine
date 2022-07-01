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
 * 
 * @return std::ifstream
 */
std::ifstream load_file(const char* path, const int mode);

} // namespace lyra
