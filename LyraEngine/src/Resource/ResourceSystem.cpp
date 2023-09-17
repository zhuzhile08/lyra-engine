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
	ResourceSystem() : assetsFile(Json::parse(StringStream(assetsFilePath(), OpenMode::readText, false).data())) {
		
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
	if (!globalResourceSystem->textures.contains(path)) {
		const auto& js = globalResourceSystem->assetsFile.at(path);
		globalResourceSystem->textures.emplace(path, loadTextureFile(
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

	return globalResourceSystem->textures.at(path);
}

const MeshFile& mesh(std::filesystem::path path) {
	if (!globalResourceSystem->meshes.contains(path)) {
		// globalResourceSystem->meshes.emplace(path, loadMeshFile(absolutePath(path)));
		globalResourceSystem->meshes.emplace(path, MeshFile{});
	}

	return globalResourceSystem->meshes.at(path);
}

const MaterialFile& material(std::filesystem::path path) {
	if (!globalResourceSystem->materials.contains(path)) {
		// globalResourceSystem->materials.emplace(path, loadMaterialFile(absolutePath(path)));
		globalResourceSystem->materials.emplace(path, MaterialFile{});
	}

	return globalResourceSystem->materials.at(path);
}

}

} // namespace lyra
