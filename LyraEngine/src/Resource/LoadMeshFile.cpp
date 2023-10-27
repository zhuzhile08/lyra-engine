#include <Resource/LoadMeshFile.h>

#include <Common/Logger.h>
#include <Common/FileSystem.h>

#include <lz4.h>

namespace lyra {

namespace resource {

MeshFile loadMeshFile(std::filesystem::path path, const std::vector <uint32>& vertexBlocks, const std::vector <uint32>& indexBlocks) {
	ByteFile compressedFile(path.concat(".dat"), OpenMode::read | OpenMode::binary, false);
	std::vector<char> fileData(compressedFile.seekg(0, SeekDirection::end).tellg());
	compressedFile.seekg(0).read(fileData.data(), fileData.size());

	std::vector<char> file(fileData.size() * sizeof(char) * 255);
	LZ4_decompress_safe(fileData.data(), file.data(), fileData.size(), file.size());

	MeshFile meshes {
		vertexBlocks,
		indexBlocks
	};

	meshes.vertexData.resize(vertexBlocks.size());
	meshes.indexData.resize(vertexBlocks.size());

	uint32 currentOffset = 0;

	for (uint32 i = 0; i < vertexBlocks.size(); i++) {
		meshes.vertexData[i].resize(vertexBlocks[i]);
		std::copy(
			file.begin() + currentOffset, 
			file.begin() + currentOffset + vertexBlocks[i] * sizeof(float) * 3 * 4, 
			meshes.vertexData[i].data()
		);
		currentOffset += vertexBlocks[i] * sizeof(float) * 3 * 4;
	}

	for (uint32 i = 0; i < vertexBlocks.size(); i++) {
		meshes.indexData[i].resize(indexBlocks[i]);
		std::copy(
			file.begin() + currentOffset, 
			file.begin() + currentOffset + indexBlocks[i] * sizeof(uint32), 
			meshes.indexData[i].data()
		);
		currentOffset += indexBlocks[i] * sizeof(uint32);
	}
	
	return meshes;
}

} // namespace resource

} // namespace lyra
