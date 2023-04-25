#include <Resource/ResourceManager.h>

#include <utility>

#include <Resource/Texture.h>
#include <Resource/Material.h>
#include <Resource/Mesh.h>

#include <Resource/LoadImage.h>
#include <Resource/LoadMaterial.h>
#include <Resource/LoadMesh.h>

namespace lyra {

Texture* ResourceManager::texture(std::string_view path) {
	if (!m_textures.contains(path.data())) {
		m_textures.emplace(std::make_pair(path, SmartPointer<Texture>::create(util::load_image(path.data()))));
	}
	return m_textures[path.data()];
}

Material* ResourceManager::material(std::string_view path) {
	if (!m_materials.contains(path.data())) {
		m_materials.emplace(std::make_pair(path, SmartPointer<Material>::create(util::load_material(path.data()))));
	}
	return m_materials[path.data()];
}

Mesh* ResourceManager::mesh(std::string_view path) {
	if (!m_meshes.contains(path.data())) {
		m_meshes.emplace(std::make_pair(path, SmartPointer<Mesh>::create(util::load_mesh(path))));
	}
	return m_meshes[path.data()];
}

std::unordered_map<std::string, SmartPointer<Texture>> ResourceManager::m_textures;
std::unordered_map<std::string, SmartPointer<Material>> ResourceManager::m_materials;
std::unordered_map<std::string, SmartPointer<Mesh>> ResourceManager::m_meshes;

} // namespace lyra
