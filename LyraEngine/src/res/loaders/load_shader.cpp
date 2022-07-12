#include <res/loaders/load_shader.h>

namespace lyra {

// load files in binary format in only read mode
void read_shader_binary(const char* const path, std::vector <char>& data) {
	// load the file
	std::ifstream file; load_file(path, OpenMode::MODE_BINARY | OpenMode::MODE_START_AT_END, file);

	size_t size = (size_t)file.tellg();

	data.resize(size);

	file.read(data.data(), size);
	file.close();
}

}
