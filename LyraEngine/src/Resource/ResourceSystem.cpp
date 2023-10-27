#include <Resource/ResourceSystem.h>

#include <Common/Logger.h>
#include <Common/FileSystem.h>

#include <Json/Json.h>

#include <utility>
#include <unordered_map>

namespace lyra {

namespace {

class ResourceSystem {
public:
	ResourceSystem() : assetsFile(Json::parse(StringStream(assetsFilePath(), OpenMode::read, false).data())) {
		
	}

	std::unordered_map<std::string, resource::TextureFile> textures;
	std::unordered_map<std::string, resource::MeshFile> meshes;
	std::unordered_map<std::string, resource::MaterialFile> materials;

	Json assetsFile;
};

}

static ResourceSystem* globalResourceSystem = nullptr;

void initResourceSystem() {
	if (globalResourceSystem) {
		log::error("lyra::initResourceSystem(): The resource system was already initialized!");
		return;
	}

	globalResourceSystem = new ResourceSystem();
}

namespace resource {

const TextureFile& texture(std::filesystem::path path) {
	if (!globalResourceSystem->textures.contains(path.string())) {
		const auto& js = globalResourceSystem->assetsFile.at(path.string());
		globalResourceSystem->textures.emplace(path.string(), loadTextureFile(
			absolutePath(std::filesystem::path("data")/(path)),
			js.at("Width").get<uint32>(),
			js.at("Height").get<uint32>(),
			js.at("Type").get<uint32>(),
			js.at("Alpha").get<uint32>(),
			js.at("Mipmap").get<uint32>(),
			js.at("Dimension").get<uint32>(),
			js.at("Wrap").get<uint32>()
		));
	}

	return globalResourceSystem->textures.at(path.string());
}

const MeshFile& mesh(std::filesystem::path path) {
	if (!globalResourceSystem->meshes.contains(path.string())) {
		const auto& js = globalResourceSystem->assetsFile.at(path.string());
		const auto& vertexBlocksJs = js.at("VertexBlocks").get<Json::array_type>();
		const auto& indexBlocksJs = js.at("IndexBlocks").get<Json::array_type>();

		// convert the json arrays to integer arrays
		std::vector<uint32> vertexBlocks(vertexBlocksJs.size());
		std::vector<uint32> indexBlocks(indexBlocksJs.size());

		for (const auto& i : vertexBlocksJs) {
			vertexBlocks.push_back(i->get<uint32>());
		}

		for (const auto& i : indexBlocksJs) {
			indexBlocks.push_back(i->get<uint32>());
		}

		globalResourceSystem->meshes.emplace(path, loadMeshFile(
			absolutePath(std::filesystem::path("data")/(path)),
			vertexBlocks,
			indexBlocks
		));
		globalResourceSystem->meshes.emplace(path.string(), MeshFile{});
	}

	return globalResourceSystem->meshes.at(path.string());
}

const MaterialFile& material(std::filesystem::path path) {
	if (!globalResourceSystem->materials.contains(path.string())) {
		// globalResourceSystem->materials.emplace(path, loadMaterialFile(absolutePath(path)));
		globalResourceSystem->materials.emplace(path.string(), MaterialFile{});
	}

	return globalResourceSystem->materials.at(path.string());
}

}

} // namespace lyra
