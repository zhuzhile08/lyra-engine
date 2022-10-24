/*************************
 * @file core/assets.h
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

#include <core/smart_pointer.h>

#include <res/loaders/load_assets.h>

namespace lyra {

class Assets {
public:
	// decompressed texture information
	struct TextureInfo {
		// texture width
		uint32 width;
		// texture height
		uint32 height;
		// texture length, exclusive to 3D images/textures
		uint32 length;
		// mipmapping levels
		uint32 mipmap;
		// type of texture
		unsigned int type;
		// how to treat the alpha value of the image
		unsigned int alpha;
		// how the UVs should read the image
		unsigned int dimension;
		// how to wrap the image if the UVs exceeds the border of the image
		unsigned int wrap;
		// anistropic filtering
		unsigned int anistropy;
		// image data
		void* data;
	}; // this also roughly represents the texture data file

	// mesh information
	struct MeshInfo {

	};

	/**
	 * @brief destructor of the window
	**/
	~Assets() noexcept { }

	Assets() noexcept = delete;
	Assets(const Assets&) noexcept = delete;
	Assets operator=(const Assets&) const noexcept = delete;

	/**
	 * @brief unpack the texture data based on the path of the texture
	 * 
	 * @param path path of the texture
	 * 
	 * @return const lyra::Assets::TextureInfo&
	 */
	NODISCARD static const TextureInfo unpack_texture(const char* path);
	/**
	 * @brief return the raw image data
	 * 
	 * @return const lyra::util::AssetFile
	 */
	NODISCARD static const util::AssetFile images() noexcept { return m_images; }
	/**
	 * @brief return the null texture
	 * 
	 * @reutrn const lyra::Texture* const 
	 */
	NODISCARD  static const Texture* const nullTexture() noexcept { return m_nullTexture; }
	/**
	 * @brief return the null normal map texture
	 *
	 * @reutrn const lyra::Texture* const
	 */
	NODISCARD static const Texture* const nullNormal() noexcept { return m_nullNormal; }

private:
	static util::AssetFile m_images;
	static SmartPointer<Texture> m_nullTexture;
	static SmartPointer<Texture> m_nullNormal;
};

} // namespace lyra