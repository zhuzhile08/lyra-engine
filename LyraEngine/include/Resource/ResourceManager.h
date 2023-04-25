/*************************
 * @file ResourceManager.h
 *
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper the basic ResourceManager features
 *
 * @date 2022-06-19
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Lyra/Lyra.h>

#include <unordered_map>
#include <string_view>
#include <Common/SmartPointer.h>

#include <Resource/LoadResources.h>

#include <Resource/Texture.h>
#include <Resource/Mesh.h>

namespace lyra {

class ResourceManager {
public:
	ResourceManager() = default;

	/**
	 * @brief construct a resource manager
	 * 
	 * @param imagePath path for an image asset file
	 */
	ResourceManager(std::string_view imagePath) : m_images() { }

	/**
	 * @brief get an already loaded texture or load it if it doesn't exist
	 * 
	 * @param path path of the texture
	 * @return lyra::Texture*
	 */
	static Texture* texture(std::string_view path);

	/**
	 * @brief get an already loaded material or load it if it doesn't exist
	 * 
	 * @param path path of the material
	 * @return lyra::Material*
	 */
	static Material* material(std::string_view path);

	/**
	 * @brief get an already loaded mesh or load it if it doesn't exist
	 * 
	 * @param path path of the mesh
	 * @return lyra::Mesh*
	 */
	static Mesh* mesh(std::string_view path);

	/**
	 * @brief return the raw image data
	 * 
	 * @return lyra::util::AssetFile
	 */
	NODISCARD util::AssetFile images() noexcept { return m_images; }
	/**
	 * @brief return the null texture
	 * 
	 * @reutrn const lyra::Texture* const 
	 */
	NODISCARD static const Texture* const nullTexture() noexcept { return texture("data/img/Default.bmp"); }
	/**
	 * @brief return the null normal map texture
	 *
	 * @reutrn const lyra::Texture* const
	 */
	NODISCARD static const Texture* const nullNormal() noexcept { return texture("data/img/Normal.bmp"); }

private:
	util::AssetFile m_images;

	static std::unordered_map<std::string, SmartPointer<Texture>> m_textures;
	static std::unordered_map<std::string, SmartPointer<Material>> m_materials;
	static std::unordered_map<std::string, SmartPointer<Mesh>> m_meshes;

	friend class Texture;
	friend class CubemapBase;
};

} // namespace lyra