#include <Resource/LoadMeshFile.h>

#include <Common/Logger.h>
#include <Common/FileSystem.h>

#include <lz4.h>

namespace lyra {

namespace resource {

MeshFile loadMeshFile(std::filesystem::path path, const Json::array_type& vertexBlocks, const Json::array_type& indexBlocks) {
	ByteFile compressedFile(path.concat(".dat"), OpenMode::read | OpenMode::binary, false);
	std::vector<char> fileData(compressedFile.seekg(0, SeekDirection::end).tellg());
	compressedFile.seekg(0).read(fileData.data(), fileData.size());

	std::vector<char> file(fileData.size() * sizeof(char) * 255);
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
		// std::copy(file.begin() + currentOffset, file.begin() + currentOffset + size * sizeof(float) * 3 * 4, meshes.vertexData[i].data());
		std::memcpy(&meshes.vertexData[i][0], &file[currentOffset], size * sizeof(float) * 3 * 4);
		currentOffset += size * sizeof(float) * 3 * 4;
	}

	for (uint32 i = 0; i < indexBlocks.size(); i++) {
		const auto& size = indexBlocks[i]->get<uint32>();
		meshes.indexBlocks[i] = size;

		meshes.indexData[i].resize(size);
		// std::copy(file.begin() + currentOffset, file.begin() + currentOffset + size * sizeof(uint32), meshes.indexData[i].data());
		std::memcpy(&meshes.indexData[i][0], &file[currentOffset], size * sizeof(uint32));
		currentOffset += size * sizeof(uint32);
	}
	
	return meshes;
}

} // namespace resource

} // namespace lyra
