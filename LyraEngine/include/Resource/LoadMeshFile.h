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

#include <Common/JSON.h>

#include <Common/Vector.h>
#include <string_view>
#include <filesystem>

namespace lyra {

namespace resource {

struct MeshFile {
	Vector<uint32> vertexBlocks;
	Vector<uint32> indexBlocks;

	Vector<Vector<float32>> vertexData;
	Vector<Vector<uint32>> indexData;
};

NODISCARD MeshFile loadMeshFile(
	std::filesystem::path path, 
	uint32 uncompressed,
	const Json::array_type& vertexBlocks,
	const Json::array_type& indexBlocks
);

} // namespace resource

} // namespace lyra
