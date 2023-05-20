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

#include <Common/FunctionPointer.h>
#include <Common/Manager.h>

#include <Resource/LoadImage.h>
#include <Resource/LoadMaterial.h>
#include <Resource/LoadMesh.h>

#include <Resource/LoadResources.h>

namespace lyra {

/**
 * @brief The manager for all asset related resources
 */
class ResourceManager {
private:
	struct StringToTexture {};
public:
	ResourceManager() = delete;

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
	NODISCARD static const Texture* const nullTexture() noexcept;
	/**
	 * @brief return the null normal map texture
	 *
	 * @reutrn const lyra::Texture* const
	 */
	NODISCARD static const Texture* const nullNormal() noexcept;

	static Manager<std::string, Texture, Function<util::detail::LoadedImage(std::string_view)>> textures;
	static Manager<std::string, Material, Function<util::detail::LoadedMaterial(std::string_view)>> materials;
	static Manager<std::string, Mesh, Function<util::detail::LoadedMesh(std::string_view)>> meshes;
	static Manager<uint32, vulkan::Shader, Function<std::pair<std::string_view, uint32>(uint32)>> shaders;

private:
	util::AssetFile m_images;

	friend class Texture;
	friend class CubemapBase;
};

} // namespace lyra