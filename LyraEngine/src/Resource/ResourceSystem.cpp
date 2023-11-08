#include <Resource/ResourceSystem.h>

#include <Common/Logger.h>
#include <Common/FileSystem.h>

#include <Json/Json.h>

#include <Resource/LoadTextureFile.h>
#include <Resource/LoadMaterialFile.h>
#include <Resource/LoadMeshFile.h>

#include <utility>
#include <unordered_map>

namespace lyra {

namespace {

class ResourceSystem {
public:
	ResourceSystem() : assetsFile(Json::parse(StringStream(assetsFilePath(), OpenMode::read, false).data())) {
		
	}

	std::unordered_map<std::string, vulkan::Shader> shaders;
	std::unordered_map<std::string, Texture> textures;
	std::unordered_map<std::string, std::vector<Mesh>> meshes;
	std::unordered_map<std::string, Material> materials;

	Json assetsFile;
};

static ResourceSystem* globalResourceSystem = nullptr;

}

void initResourceSystem() {
	if (globalResourceSystem) {
		log::error("lyra::initResourceSystem(): The resource system was already initialized!");
		return;
	}

	globalResourceSystem = new ResourceSystem();
}

namespace resource {

const vulkan::Shader& shader(std::filesystem::path path) {
	if (!globalResourceSystem->shaders.contains(path.string())) {
		const auto& js = globalResourceSystem->assetsFile.at(path.string());

		ByteFile compressedFile(absolutePath(std::filesystem::path("data")/(path)), OpenMode::read | OpenMode::binary, false);
		std::vector<char> data(compressedFile.seekg(0, SeekDirection::end).tellg());
		compressedFile.seekg(0).read(data.data(), data.size());

		globalResourceSystem->shaders.emplace(path.string(), vulkan::Shader(
			static_cast<vulkan::Shader::Type>(js.at("Type").get<uint32>()), 
			data
		));
	}

	return globalResourceSystem->shaders.at(path.string());
}

const Texture& texture(std::filesystem::path path) {
	if (!globalResourceSystem->textures.contains(path.string())) {
		const auto& js = globalResourceSystem->assetsFile.at(path.string());
		globalResourceSystem->textures.emplace(path.string(), Texture(loadTextureFile(
			absolutePath(std::filesystem::path("data")/(path)),
			js.at("Width").get<uint32>(),
			js.at("Height").get<uint32>(),
			js.at("Type").get<uint32>(),
			js.at("Alpha").get<uint32>(),
			js.at("Mipmap").get<uint32>(),
			js.at("Dimension").get<uint32>(),
			js.at("Wrap").get<uint32>()
		)));
	}

	return globalResourceSystem->textures.at(path.string());
}

const std::vector<Mesh>& mesh(std::filesystem::path path) {
	if (!globalResourceSystem->meshes.contains(path.string())) {
		const auto& js = globalResourceSystem->assetsFile.at(path.string());
		const auto& vertexBlocks = js.at("VertexBlocks").get<Json::array_type>();
		const auto& indexBlocks = js.at("IndexBlocks").get<Json::array_type>();

		auto meshData = loadMeshFile(
			absolutePath(std::filesystem::path("data")/(path)),
			vertexBlocks,
			indexBlocks
		);
		auto& vec = globalResourceSystem->meshes.emplace(path.string(), std::vector<Mesh>(vertexBlocks.size())).first->second;

		for (uint32 i = 0; i < vertexBlocks.size(); i++) {
			vec.emplace_back(meshData, i);
		}
	}

	return globalResourceSystem->meshes.at(path.string());
}

const Material& material(std::filesystem::path path) {
	if (!globalResourceSystem->materials.contains(path.string())) {
		// globalResourceSystem->materials.emplace(path, loadMaterialFile(absolutePath(path)));
		// globalResourceSystem->materials.emplace(path.string(), MaterialFile{});
	}

	return globalResourceSystem->materials.at(path.string());
}

} // namespace resource

} // namespace lyra
