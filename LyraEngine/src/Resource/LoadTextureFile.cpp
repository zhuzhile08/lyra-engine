#include <Resource/LoadTextureFile.h>

#include <Common/FileSystem.h>
#include <Common/Logger.h>

#include <lz4.h>

namespace lyra {

namespace resource {

TextureFile loadTextureFile(
	std::filesystem::path path, 
	uint32 width,
	uint32 height,
	uint32 type,
	uint32 alpha,
	uint32 mipmap,
	uint32 dimension,
	uint32 wrap
) {
	ByteFile compressedFile(path.concat(".dat"), OpenMode::readBin, false);
	std::vector<char> fileData(compressedFile.seekg(0, SeekDirection::end).tellg());
	compressedFile.seekg(0).read(fileData.data(), fileData.size());

	std::vector<char> file(LZ4_decompress_safe(fileData.data(), nullptr, fileData.size(), 0));
	LZ4_decompress_safe(fileData.data(), file.data(), fileData.size(), file.size());

	TextureFile data {
		width,
		height,
		type,
		alpha,
		mipmap,
		dimension,
		wrap
	};
	data.data.resize(file.size());
	std::copy(file.begin(), file.end(), data.data.data()); // slightly cursed

	return data;
}

} // namespace resource

} // namespace lyra
