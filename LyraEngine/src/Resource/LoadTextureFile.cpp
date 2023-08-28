#include <Resource/LoadTextureFile.h>

#include <Common/FileSystem.h>
#include <Common/Logger.h>

#include <lz4.h>

namespace lyra {

namespace resource {

TextureFile loadImage(std::string_view path) {
	ByteFile compressedFile(path, OpenMode::readBin, false);
	std::vector<char> fileData(compressedFile.seekg(0, SeekDirection::end).tellg());
	compressedFile.seekg(0).read(fileData.data(), fileData.size());

	std::vector<char> file(LZ4_decompress_safe(fileData.data(), nullptr, fileData.size(), 0));
	LZ4_decompress_safe(fileData.data(), file.data(), fileData.size(), file.size());

	TextureFile data;
	data.data.resize(file.size() - 36);
	std::copy(file.begin(), file.end(), reinterpret_cast<char*>(&data)); // slightly cursed

	return data;
}

} // namespace resource

} // namespace lyra
