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

#include <Lyra/Lyra.h>

#include <vector>
#include <tiny_obj_loader.h>

namespace lyra {

namespace util {

/**
 * @brief a struct holding some "freshly" loaded mesh data
 */
struct LoadedMesh {
	// raw model directly after loaded with TOL
	struct TOLMesh {
		tinyobj::attrib_t vertices;	
		std::vector<tinyobj::shape_t> shapes;
	};
	// raw vertex data
	struct Vertex {
		glm::vec3 pos = glm::vec3(1.0f);
		glm::vec3 normal = glm::vec3(1.0f);
		glm::vec3 color = glm::vec3(1.0f);
		glm::vec3 uvw = glm::vec3(1.0f);
	};

	std::string path;
	std::vector<uint32> indices;
	std::vector<Vertex> vertices;
};

/**
 * @brief load a mesh from an .obj file
 *
 * @param path path of the mesh
 * 
 * @return lyra::util::LoadedMesh
 */
NODISCARD LoadedMesh load_mesh(std::string_view path);

} // namespace util

} // namespace lyra
