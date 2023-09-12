#include <Resource/LoadMeshFile.h>

#include <Common/Logger.h>
#include <Common/FileSystem.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

namespace lyra {

namespace resource {

MeshFile loadMesh(std::string_view path, MeshFlags flags) {
	Assimp::Importer importer;
	importer.SetIOHandler(new AssimpFileSystem);

	const aiScene* scene = importer.ReadFile(path.data(), static_cast<uint32>(flags));

	if (!scene) {
		log::error("An error occurred while loading a material: {}!", importer.GetErrorString());
	}

	
	
	return mesh;
}

} // namespace resource

} // namespace lyra
