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

#include <vector>
#include <string_view>
#include <filesystem>

namespace lyra {

namespace resource {

struct MeshFile {
	struct Vertex {
		glm::vec3 pos = glm::vec3(1.0f);
		glm::vec3 normal = glm::vec3(1.0f);
		glm::vec3 color = glm::vec3(1.0f);
		glm::vec3 uvw = glm::vec3(1.0f);
	};

	std::vector<uint32> indices;
	std::vector<Vertex> vertices;
};

NODISCARD MeshFile loadMeshFile(std::filesystem::path path);

} // namespace resource

} // namespace lyra
