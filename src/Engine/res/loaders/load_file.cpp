#include <res/loaders/load_file.h>

namespace lyra {

// load files in binary format in only read mode
std::vector <char> read_binary(std::string path) {
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	LOG_DEBUG(TAB, "Loaded file in binary format at path ", path)

	if (!file.is_open()) {
		LOG_EXEPTION("Failed to open shader in binary format at path ", path)
	}

	size_t size = (size_t)file.tellg();

	std::vector <char> shader(size);

	file.seekg(0);
	file.read(shader.data(), size);
	file.close();

	return shader;
}

std::vector <char> read_text(std::string path) {
	std::ifstream file(path, std::ios::ate);

	LOG_DEBUG(TAB, "Loaded file in text format at path ", path)

	if (!file.is_open()) {
		LOG_EXEPTION("Failed to open shader in text format at path ", path)
	}

	size_t size = (size_t)file.tellg();

	std::vector <char> shader(size);

	file.seekg(0);
	file.read(shader.data(), size);
	file.close();

	return shader;
}

}
