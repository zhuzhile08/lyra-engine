#include <Resource/LoadTextureFile.h>

#include <Common/FileSystem.h>
#include <Common/Logger.h>

#include <Common/Benchmark.h>

#include <lz4.h>

namespace lyra {

namespace resource {

TextureFile loadTextureFile(
	std::filesystem::path path,
	uint32 uncompressed,
	uint32 width,
	uint32 height,
	uint32 type,
	uint32 alpha,
	uint32 mipmap,
	uint32 dimension,
	uint32 wrap
) {
	ByteFile compressedFile(path.concat(".dat"), OpenMode::read | OpenMode::binary, false);
	lsd::Vector<char> fileData(compressedFile.size());
	compressedFile.read(fileData.data(), fileData.size());

	TextureFile data {
		width,
		height,
		type,
		alpha,
		mipmap,
		dimension,
		wrap,
		{ }
	};
	
	data.data.resize(uncompressed);
	LZ4_decompress_safe(fileData.data(), data.data.data(), static_cast<uint32>(fileData.size()), static_cast<uint32>(data.data.size()));

	return data;
}

} // namespace resource

} // namespace lyra
