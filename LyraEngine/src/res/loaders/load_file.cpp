#include <res/loaders/load_file.h>

#include <filesystem>

namespace lyra {

namespace util {

void load_file(const char* path, const int mode, std::ifstream& file) {
	// get the absolute path of the relative path
	std::filesystem::path absPath(get_executable_path() / path);

	// load the binary
	file.open(absPath, static_cast<std::ios_base::openmode>(mode));
	lassert(file.good(), "Failed to open a file for reading at path: ", path, "!");
	Logger::log_debug(Logger::tab(), "Successfully loaded file in at path ", path, "!");
}

} // namespace util

} // namespace lyra
