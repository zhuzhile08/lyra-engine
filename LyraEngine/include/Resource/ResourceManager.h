/*************************
 * @file ResourceManager.h
 *
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper the basic Assets features
 *
 * @date 2022-06-19
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Lyra/Lyra.h>

#include <unordered_map>

#include <Common/SmartPointer.h>

#include <Resource/LoadResources.h>

#include <Graphics/Texture.h>

namespace lyra {

class Assets {
public:
	Assets() = default;

	/**
	 * @brief construct a Assets manager
	 * 
	 * @param imagepath path for an image asset file
	 */
	Assets(std::string_view imagePath) : m_images() { }
	
	~Assets() = default;

	Assets(const Assets&) noexcept = delete;
	Assets operator=(const Assets&) const noexcept = delete;

	/**
	 * @brief get an already loaded texture from the map
	 * 
	 * @param path path of the texture
	 * @return const Texture* const 
	 */
	const Texture* const at(const char* const path) const {
		return m_textures.at(path);
	}
	/**
	 * @brief get an already loaded texture from the map
	 * 
	 * @param path path of the texture
	 * @return Texture*
	 */
	Texture* operator[](const char* const path);
	/**
	 * @brief get an already loaded texture from the map
	 * 
	 * @param path path of the texture
	 * @return const Texture* const
	 */
	const Texture* const operator[](const char* const path) const;

	/**
	 * @brief return the raw image data
	 * 
	 * @return const lyra::util::AssetFile
	 */
	NODISCARD const util::AssetFile images() noexcept { return m_images; }
	/**
	 * @brief return the null texture
	 * 
	 * @reutrn const lyra::Texture* const 
	 */
	NODISCARD static const Texture* const nullTexture() noexcept { return &m_nullTexture; }
	/**
	 * @brief return the null normal map texture
	 *
	 * @reutrn const lyra::Texture* const
	 */
	NODISCARD static const Texture* const nullNormal() noexcept { return &m_nullNormal; }

private:
	util::AssetFile m_images;

	static std::unordered_map<const char*, SmartPointer<Texture>> m_textures;

	static Texture m_nullTexture;
	static Texture m_nullNormal;

	/**
	 * @brief unpack the texture data based on the path of the texture
	 * 
	 * @param path path of the texture
	 * 
	 * @return lyra::util::ImageData
	 */
	NODISCARD static util::ImageData unpack_texture(std::string_view path);

	friend class Texture;
	friend class CubemapBase;
};

} // namespace lyra