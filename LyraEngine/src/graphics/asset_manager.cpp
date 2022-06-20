#include <graphics/asset_manager.h>

namespace lyra {

void AssetManager::AssetFile::load(std::string path) {
	std::ofstream outfile;
	outfile.open(path, std::ios::binary | std::ios::out);

	if (!outfile.is_open()) Logger::log_exception("Failed to open an asset file for reading at path: ", path, "!");

	Logger::log_debug(Logger::tab(), "Successfully loaded asset file in at path ", path, "!");

	outfile.write(type, 4);
	uint32_t _version = version;
	//version
	outfile.write((const char*)&version, sizeof(uint32_t));

	//json length
	uint32_t length = json.size();
	outfile.write((const char*)&length, sizeof(uint32_t));

	//blob length
	uint32_t binLength = binary.size();
	outfile.write((const char*)&binLength, sizeof(uint32_t));

	//json stream
	outfile.write(json.data(), length);
	//blob data
	outfile.write(binary.data(), binary.size());

	outfile.close();
}

void AssetManager::AssetFile::store(std::string path) {
	std::ifstream infile;
	infile.open(path, std::ios::binary);

	if (!infile.is_open()) Logger::log_exception("Failed to open an asset file for writing at path: ", path, "!");

	Logger::log_debug(Logger::tab(), "Successfully loaded asset file in at path ", path, "!");

	//move file cursor to beginning
	infile.seekg(0);

	infile.read(type, 4);
	infile.read((char*)&version, sizeof(uint32_t));

	uint32_t jsonlen = 0;
	infile.read((char*)&jsonlen, sizeof(uint32_t));

	uint32_t binLen = 0;
	infile.read((char*)&binLen, sizeof(uint32_t));

	json.resize(jsonlen);
	infile.read(json.data(), jsonlen);

	binary.resize(binLen);
	infile.read(binary.data(), binLen);
}

} // namespace lyra
