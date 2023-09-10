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
	uint32 length;
	uint32 mipmap;
	uint32 type;
	uint32 alpha;
	uint32 dimension;
	uint32 wrap;
	uint32 anistropy;

	std::vector<uint8> data;
};

NODISCARD TextureFile loadImage(const std::filesystem::path& path);

} // namespace resource

} // namespace lyra