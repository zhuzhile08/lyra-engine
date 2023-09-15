/*************************
 * @file LoadTexture.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Image loading functionality
 * 
 * @date 2023-04-02
 * 
 * @copyright Copyright (c) 2023
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/UniquePointer.h>

#include <vector>
#include <filesystem>

namespace lyra {

namespace resource {

struct TextureFile {
	uint32 width;
	uint32 height;
	uint32 type;
	uint32 alpha;
	uint32 mipmap;
	uint32 dimension;
	uint32 wrap;

	std::vector<uint8> data;
};

NODISCARD TextureFile loadTextureFile(
	std::filesystem::path path, 
	uint32 width,
	uint32 height,
	uint32 type,
	uint32 alpha,
	uint32 mipmap,
	uint32 dimension,
	uint32 wrap
);

} // namespace resource

} // namespace lyra
