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
	ByteFile compressedFile(path.concat(".dat"), OpenMode::read | OpenMode::binary, false);
	std::vector<char> fileData(compressedFile.seekg(0, SeekDirection::end).tellg());
	compressedFile.seekg(0).read(fileData.data(), fileData.size());

	TextureFile data {
		width,
		height,
		type,
		alpha,
		mipmap,
		dimension,
		wrap
	};
	data.data.reserve(fileData.size() * sizeof(char) * 255);

	LZ4_decompress_safe(fileData.data(), data.data.data(), fileData.size(), data.data.size());

	return data;
}

} // namespace resource

} // namespace lyra
