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
#include <Common/JSON.h>

#include <LSD/Utility.h>
#include <LSD/Vector.h>
#include <LSD/StringView.h>

#include <filesystem>

namespace lyra {

namespace resource {

struct MeshFile {
	lsd::Vector<uint32> vertexBlocks;
	lsd::Vector<uint32> indexBlocks;

	lsd::Vector<lsd::Vector<float32>> vertexData;
	lsd::Vector<lsd::Vector<uint32>> indexData;
};

NODISCARD MeshFile loadMeshFile(
	std::filesystem::path path, 
	uint32 uncompressed,
	const Json::array_type& vertexBlocks,
	const Json::array_type& indexBlocks
);

} // namespace resource

} // namespace lyra
