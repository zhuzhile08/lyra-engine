#include <res/loaders/load_file.h>

#include <filesystem>

#include <core/logger.h>

namespace lyra {

namespace util {

void load_file(const char* path, const int& mode, std::ifstream& file) {
	// get the absolute path of the relative path
#ifndef _WIN32
	std::filesystem::path absPath(get_executable_path() / path);

	// load the binary
	file.open(absPath, static_cast<std::ios_base::openmode>(mode));
#else
	file.open(path, static_cast<std::ios_base::openmode>(mode));
#endif
	lassert(file.good(), "Failed to open a file for reading at path: ", path, "!");
}

} // namespace util

} // namespace lyra
