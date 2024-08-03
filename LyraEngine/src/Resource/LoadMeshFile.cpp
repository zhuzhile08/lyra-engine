#include <Resource/LoadMeshFile.h>

#include <Common/Logger.h>
#include <Common/FileSystem.h>

#include <lz4.h>

using namespace lsd::enum_operators;

namespace lyra {

namespace resource {

MeshFile loadMeshFile(std::filesystem::path path, uint32 uncompressed, const lsd::Json::array_type& vertexBlocks, const lsd::Json::array_type& indexBlocks) {
	ByteFile compressedFile(path.concat(".dat"), OpenMode::read | OpenMode::binary, false);
	lsd::Vector<char> fileData(compressedFile.size());
	compressedFile.read(fileData.data(), fileData.size());

	lsd::Vector<char> file(uncompressed);
	LZ4_decompress_safe(fileData.data(), file.data(), static_cast<uint32>(fileData.size()), static_cast<uint32>(file.size()));

	MeshFile meshes { };

	meshes.vertexData.resize(vertexBlocks.size());
	meshes.indexData.resize(indexBlocks.size());

	meshes.vertexBlocks.resize(vertexBlocks.size());
	meshes.indexBlocks.resize(indexBlocks.size());

	uint32 currentOffset = 0;

	for (uint32 i = 0; i < vertexBlocks.size(); i++) {
		const auto& size = vertexBlocks[i]->get<uint32>();
		meshes.vertexBlocks[i] = size;

		meshes.vertexData[i].resize(size * 3 * 4);
		std::memcpy(meshes.vertexData[i].data(), &file[currentOffset], size * sizeof(float32) * 3 * 4);
		currentOffset += size * sizeof(float32) * 3 * 4;
	}

	for (uint32 i = 0; i < indexBlocks.size(); i++) {
		const auto& size = indexBlocks[i]->get<uint32>();
		meshes.indexBlocks[i] = size;

		meshes.indexData[i].resize(size);
		std::memcpy(meshes.indexData[i].data(), &file[currentOffset], size * sizeof(uint32));
		currentOffset += size * sizeof(uint32);
	}
	
	return meshes;
}

} // namespace resource

} // namespace lyra
