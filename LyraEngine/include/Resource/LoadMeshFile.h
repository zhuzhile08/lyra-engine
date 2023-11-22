/*************************
 * @file LoadMesh.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Mesh loading functionality
 * 
 * @date 2023-04-02
 * 
 * @copyright Copyright (c) 2023
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/Utility.h>

#include <Json/Json.h>

#include <vector>
#include <string_view>
#include <filesystem>

namespace lyra {

namespace resource {

struct MeshFile {
	std::vector <uint32> vertexBlocks;
	std::vector <uint32> indexBlocks;

	std::vector <std::vector <float>> vertexData;
	std::vector <std::vector <uint32>> indexData;
};

NODISCARD MeshFile loadMeshFile(
	std::filesystem::path path, 
	const Json::array_type& vertexBlocks,
	const Json::array_type& indexBlocks
);

} // namespace resource

} // namespace lyra
