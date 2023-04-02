#include <Resource/LoadFile.h>

#include <filesystem>

#include <Common/Logger.h>

#include <Common/Utility.h>

namespace lyra {

namespace util {

void load_file(std::string_view path, const int& mode, std::ifstream& file) {
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
