#include <Resource/ResourceSystem.h>

#include <Common/FileSystem.h>

#include <Json/Json.h>

#include <utility>
#include <unordered_map>

namespace lyra {

namespace resource {

namespace {

class ResourceSystem {
public:
    ResourceSystem() : assetsFile(Json::parse(StringStream(assetsFilePath(), OpenMode::readText, false).data())) {
        
    }

    std::unordered_map<std::filesystem::path, TextureFile> textures;
    std::unordered_map<std::filesystem::path, MeshFile> meshes;
    std::unordered_map<std::filesystem::path, MaterialFile> materials;

    Json assetsFile;
};

}

static ResourceSystem* resourceManager;

void initResourceSystem() {

}


const TextureFile& texture(const std::filesystem::path& name) {

}

const MeshFile& mesh(const std::filesystem::path& name) {

}

const MaterialFile& material(const std::filesystem::path& name) {

}

}

} // namespace lyra
