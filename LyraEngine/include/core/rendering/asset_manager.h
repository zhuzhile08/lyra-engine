/*************************
 * @file core/asset_manager.h
 *
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper the basic AssetManager features
 *
 * @date 2022-06-19
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/logger.h>
#include <res/loaders/load_assets.h>

#include <stb_image.h>
#include <vector>

namespace lyra {

namespace non_access {

} // namespace non_access

class AssetManager {
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
	~AssetManager() noexcept { }

	AssetManager() noexcept = delete;
	AssetManager(const AssetManager&) noexcept = delete;
	AssetManager operator=(const AssetManager&) const noexcept = delete;
	/**
	 * @brief initialize the asset manager and load all the raw data
	 */
	static void init();

	/**
	 * @brief unpack the texture data based on the path of the texture
	 * 
	 * @param path path of the texture
	 * 
	 * @return const lyra::AssetManager::TextureInfo&
	 */
	[[nodiscard]] static const TextureInfo unpack_texture(const char* path);

	/**
	 * @brief return the raw image data
	 * 
	 * @return const lyra::non_access::AssetFile&
	 */
	[[nodiscard]] static const non_access::AssetFile& images() noexcept { return _images; }

private:
	static non_access::AssetFile _images;
};

} // namespace lyra