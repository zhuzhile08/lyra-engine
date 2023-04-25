/*************************
 * @file LoadImage.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Image loading functionality
 * 
 * @date 2023-04-02
 * 
 * @copyright Copyright (c) 2023
 *************************/

#pragma once

#include <Lyra/Lyra.h>

#include <string_view>

namespace lyra {

namespace util {

// decompressed texture information
struct ImageData {
	// texture width
	uint32 width;
	// texture height
	uint32 height;
	// texture length, exclusive to 3D images/textures
	uint32 length;
	// mipmapping levels
	uint32 mipmap;
	// type of texture
	uint32 type;
	// how to treat the alpha value of the image
	uint32 alpha;
	// how the UVs should read the image
	uint32 dimension;
	// how to wrap the image if the UVs exceeds the border of the image
	uint32 wrap;
	// anistropic filtering
	uint32 anistropy;
	// image data
	void* data;
}; // this also roughly represents the texture data file

/**
 * @brief load an image from disc via path
 * 
 * @param path path of the image to load
 * @return lyra::util::ImageData
 */
NODISCARD ImageData load_image(std::string_view path);

} // namespace util

} // namespace lyra
