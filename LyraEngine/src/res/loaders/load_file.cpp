#include <res/loaders/load_file.h>

namespace lyra {

const std::ifstream load_file(const char* const path, const int mode) {
	// load the binary
	std::ifstream file;
	file.open(path, static_cast<std::ios_base::openmode>(mode));
	if (!file.is_open()) Logger::log_exception("Failed to open a file for reading at path: ", path, "!");
	Logger::log_debug(Logger::tab(), "Successfully loaded file in at path ", path, "!");

	// move the cursor to the beginning of the file
	file.seekg(0);

	return file
}

} // namespace lyra
