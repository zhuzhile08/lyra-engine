#include <Resource/ResourceManager.h>

#include <utility>

#include <Resource/Texture.h>
#include <Resource/Material.h>
#include <Resource/Mesh.h>
#include <Resource/Shader.h>

#include <Resource/LoadImage.h>
#include <Resource/LoadMaterial.h>
#include <Resource/LoadMesh.h>

namespace lyra {

Texture* const ResourceManager::texture(std::string_view path) {
	if (!m_textures.contains(path.data())) {
		m_textures.emplace(std::make_pair(path, SmartPointer<Texture>::create(util::load_image(path.data()))));
	}
	return m_textures[path.data()];
}

Material* const ResourceManager::material(std::string_view path) {
	if (!m_materials.contains(path.data())) {
		m_materials.emplace(std::make_pair(path, SmartPointer<Material>::create(util::load_material(path.data()))));
	}
	return m_materials[path.data()];
}

Mesh* const ResourceManager::mesh(std::string_view path) {
	if (!m_meshes.contains(path.data())) {
		m_meshes.emplace(std::make_pair(path, SmartPointer<Mesh>::create(util::load_mesh(path))));
	}
	return m_meshes[path.data()];
}

vulkan::Shader* const ResourceManager::shader(std::string_view path) {
	if (!m_shaders.contains(path.data())) {
		m_shaders.emplace(std::make_pair(path, SmartPointer<Mesh>::create(util::load_mesh(path))));
	}
	return m_shaders[path.data()];
}

std::unordered_map<std::string, SmartPointer<Texture>> ResourceManager::m_textures;
std::unordered_map<std::string, SmartPointer<Material>> ResourceManager::m_materials;
std::unordered_map<std::string, SmartPointer<Mesh>> ResourceManager::m_meshes;
std::unordered_map<std::string, SmartPointer<vulkan::Shader>> ResourceManager::m_shaders;

} // namespace lyra
