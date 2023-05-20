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

namespace detail {

struct LoadedImage {
	uint32 width;
	uint32 height;
	uint32 length;
	uint32 mipmap;
	uint32 type;
	uint32 alpha;
	uint32 dimension;
	uint32 wrap;
	uint32 anistropy;
	void* data;
};

} // namespace detail

/**
 * @brief load an image from disc via path
 * 
 * @param path path of the image to load
 * @return lyra::util::detail::LoadedImage
 */
NODISCARD detail::LoadedImage load_image(std::string_view path);

} // namespace util

} // namespace lyra
