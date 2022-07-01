#include <res/loaders/load_shader.h>

namespace lyra {

// load files in binary format in only read mode
std::vector <char>& read_shader_binary(const char* path) {
	// load the file
	std::ifstream file = load_file(path, OpenMode::MODE_BINARY | OpenMode::MODE_START_AT_END);

	size_t size = (size_t)file.tellg();

	std::vector <char> shader(size);

	file.read(shader.data(), size);
	file.close();

	return shader;
}

}
