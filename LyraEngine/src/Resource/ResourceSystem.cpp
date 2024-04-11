#include <Resource/ResourceSystem.h>

#include <Common/Logger.h>
#include <Common/FileSystem.h>

#include <Common/JSON.h>

#include <Resource/LoadTextureFile.h>
#include <Resource/LoadMaterialFile.h>
#include <Resource/LoadMeshFile.h>

#include <utility>
#include <Common/UnorderedSparseMap.h>

namespace lyra {

namespace {

class ResourceSystem {
public:
	ResourceSystem() : assetsFile(Json::parse(StringStream(assetsFilePath(), OpenMode::read, false).data())) {
		
	}

	UnorderedSparseMap<std::string, vulkan::Shader> shaders;
	UnorderedSparseMap<std::string, Texture> textures;
	UnorderedSparseMap<std::string, Vector<Mesh>> meshes;
	UnorderedSparseMap<std::string, Material> materials;

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
		const auto& js = globalResourceSystem->assetsFile.child(path.generic_string());

		ByteFile compressedFile(absolutePath(std::filesystem::path("data")/(path)), OpenMode::read | OpenMode::binary, false);
		Vector<char> data(compressedFile.size());
		compressedFile.read(data.data(), data.size());

		globalResourceSystem->shaders.emplace(path.string(), vulkan::Shader(
			static_cast<vulkan::Shader::Type>(js.child("Type").get<uint32>()), 
			data
		));
	}

	return globalResourceSystem->shaders.at(path.string());
}

const Texture& texture(std::filesystem::path path) {
	if (!globalResourceSystem->textures.contains(path.string())) {
		const auto& js = globalResourceSystem->assetsFile.child(path.generic_string());
		globalResourceSystem->textures.emplace(path.string(), Texture(loadTextureFile(
			absolutePath(std::filesystem::path("data")/(path)),
			js.child("Uncompressed").get<uint32>(),
			js.child("Width").get<uint32>(),
			js.child("Height").get<uint32>(),
			js.child("Type").get<uint32>(),
			js.child("Alpha").get<uint32>(),
			js.child("Mipmap").get<uint32>(),
			js.child("Dimension").get<uint32>(),
			js.child("Wrap").get<uint32>()
		)));
	}

	return globalResourceSystem->textures.at(path.string());
}

const Vector<Mesh>& mesh(std::filesystem::path path) {
	if (!globalResourceSystem->meshes.contains(path.string())) {
		const auto& js = globalResourceSystem->assetsFile.child(path.string());
		const auto& vertexBlocks = js.child("VertexBlocks").get<Json::array_type>();
		const auto& indexBlocks = js.child("IndexBlocks").get<Json::array_type>();

		auto meshData = loadMeshFile(
			absolutePath(std::filesystem::path("data")/(path)),
			js.child("Uncompressed").get<uint32>(),
			vertexBlocks,
			indexBlocks
		);
		auto& vec = globalResourceSystem->meshes.emplace(path.string(), Vector<Mesh>()).first->second;
		vec.reserve(vertexBlocks.size());

		for (uint32 i = 0; i < vertexBlocks.size(); i++) {
			vec.emplaceBack(meshData, i);
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

const Texture& defaultTexture() {
	if (!globalResourceSystem->textures.contains("defaultTexture")) {
		globalResourceSystem->textures.emplace("defaultTexture", Texture({
			1,
			1,
			0,
			1,
			1,
			1,
			0,
			{ '\xff', '\xff', '\xff' }
		}));
	}

	return globalResourceSystem->textures.at("defaultTexture");
}

const Texture& defaultNormal() {
	if (!globalResourceSystem->textures.contains("defaultNormal")) {
		globalResourceSystem->textures.emplace("defaultNormal", Texture({
			1,
			1,
			1,
			1,
			1,
			1,
			0,
			{ '\x80', '\x80', '\xff' }
		}));
	}

	return globalResourceSystem->textures.at("defaultNormal");
}

} // namespace resource

} // namespace lyra
