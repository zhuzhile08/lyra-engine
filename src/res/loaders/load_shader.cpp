#include <res/loaders/load_file.h>

namespace lyra {

// load files in binary format in only read mode
std::vector <char> read_shader_binary(std::string path) {
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		Logger::log_exception("Failed to open .spv shader in binary format at path: ", path, "!");
	}

	Logger::log_debug(Logger::tab(), "Loaded .spv shader file in binary format at path: ", path, "!");

	size_t size = (size_t)file.tellg();

	std::vector <char> shader(size);

	file.seekg(0);
	file.read(shader.data(), size);
	file.close();

	return shader;
}

}
