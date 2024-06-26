#include <Resource/ResourceSystem.h>

#include <Common/Logger.h>
#include <Common/FileSystem.h>
#include <LSD/UniquePointer.h>

#include <Common/JSON.h>

#include <Resource/LoadTextureFile.h>
#include <Resource/LoadMaterialFile.h>
#include <Resource/LoadMeshFile.h>

#include <utility>
#include <LSD/UnorderedSparseMap.h>

namespace lyra {

namespace {

class ResourceSystem {
public:
	ResourceSystem() : assetsFile(Json::parse(StringStream(assetsFilePath(), OpenMode::read, false).data())) {
		
	}

	lsd::UnorderedSparseMap<std::string, lsd::UniquePointer<vulkan::Shader>> shaders;
	lsd::UnorderedSparseMap<std::string, lsd::UniquePointer<Texture>> textures;
	lsd::UnorderedSparseMap<std::string, lsd::UniquePointer<lsd::Vector<Mesh>>> meshes;
	lsd::UnorderedSparseMap<std::string, lsd::UniquePointer<Material>> materials;

	Json assetsFile;
};

static ResourceSystem* globalResourceSystem = nullptr;

}

void initResourceSystem() {
	if (globalResourceSystem)
		log::error("lyra::initResourceSystem(): The resource system was already initialized!");
	else
		globalResourceSystem = new ResourceSystem();
}

namespace resource {

const vulkan::Shader& shader(std::filesystem::path path) {
	if (!globalResourceSystem->shaders.contains(path.string())) {
		const auto& js = globalResourceSystem->assetsFile.child(path.generic_string().c_str());

		ByteFile compressedFile(absolutePath(std::filesystem::path("data")/(path)), OpenMode::read | OpenMode::binary, false);
		lsd::Vector<char> data(compressedFile.size());
		compressedFile.read(data.data(), data.size());

		globalResourceSystem->shaders.emplace(
			path.string(),
			lsd::UniquePointer<vulkan::Shader>::create(
				static_cast<vulkan::Shader::Type>(js.child("Type").get<uint32>()), 
				data
			)
		);
	}

	return *globalResourceSystem->shaders.at(path.string());
}

const Texture& texture(std::filesystem::path path) {
	if (!globalResourceSystem->textures.contains(path.string())) {
		const auto& js = globalResourceSystem->assetsFile.child(path.generic_string().c_str());
		globalResourceSystem->textures.emplace(
			path.string(), 
			lsd::UniquePointer<Texture>::create(loadTextureFile(
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

	return *globalResourceSystem->textures.at(path.string());
}

const lsd::Vector<Mesh>& mesh(std::filesystem::path path) {
	if (!globalResourceSystem->meshes.contains(path.string())) {
		const auto& js = globalResourceSystem->assetsFile.child(path.generic_string().c_str());
		const auto& vertexBlocks = js.child("VertexBlocks").get<Json::array_type>();
		const auto& indexBlocks = js.child("IndexBlocks").get<Json::array_type>();

		auto meshData = loadMeshFile(
			absolutePath(std::filesystem::path("data")/(path)),
			js.child("Uncompressed").get<uint32>(),
			vertexBlocks,
			indexBlocks
		);
		auto vec = globalResourceSystem->meshes.emplace(path.string(), lsd::UniquePointer<lsd::Vector<Mesh>>::create()).first->second.get();
		vec->reserve(vertexBlocks.size());

		for (uint32 i = 0; i < vertexBlocks.size(); i++) {
			vec->emplaceBack(meshData, i);
		}
	}

	return *globalResourceSystem->meshes.at(path.string());
}

const Material& material(std::filesystem::path path) {
	if (!globalResourceSystem->materials.contains(path.string())) {
		// globalResourceSystem->materials.emplace(path, loadMaterialFile(absolutePath(path)));
		// globalResourceSystem->materials.emplace(path.string(), MaterialFile{});
	}

	return *globalResourceSystem->materials.at(path.string());
}

const Texture& defaultTexture() {
	if (!globalResourceSystem->textures.contains("defaultTexture")) {
		globalResourceSystem->textures.emplace("defaultTexture", lsd::UniquePointer<Texture>::create(TextureFile{
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

	return *globalResourceSystem->textures.at("defaultTexture");
}

const Texture& defaultNormal() {
	if (!globalResourceSystem->textures.contains("defaultNormal")) {
		globalResourceSystem->textures.emplace("defaultNormal", lsd::UniquePointer<Texture>::create(TextureFile{
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

	return *globalResourceSystem->textures.at("defaultNormal");
}

} // namespace resource

} // namespace lyra
