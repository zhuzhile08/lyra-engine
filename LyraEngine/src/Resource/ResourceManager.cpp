#include <Resource/ResourceManager.h>

#include <utility>

#include <Resource/Shader.h>
#include <Resource/Texture.h>
#include <Resource/Material.h>
#include <Resource/Mesh.h>

namespace lyra {

namespace {

std::pair<std::string_view, uint32> convert_to_shader_param(uint32 key) {
    return {"shader" + std::to_string(key) + ".spv", key};
}

}

const Texture* const ResourceManager::nullTexture() noexcept { return textures["data/img/Default.bmp"]; }
const Texture* const ResourceManager::nullNormal() noexcept { return textures["data/img/Normal.bmp"]; }

Manager<std::string, Texture, Function<util::detail::LoadedImage(std::string_view)>> ResourceManager::textures(util::load_image);
Manager<std::string, Material, Function<util::detail::LoadedMaterial(std::string_view)>> ResourceManager::materials(util::load_material);
Manager<std::string, Mesh, Function<util::detail::LoadedMesh(std::string_view)>> ResourceManager::meshes(util::load_mesh);
Manager<uint32, vulkan::Shader, Function<std::pair<std::string_view, uint32>(uint32)>> ResourceManager::shaders(convert_to_shader_param);

} // namespace lyra
