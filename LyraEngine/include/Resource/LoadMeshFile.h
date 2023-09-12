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
// #include <tiny_obj_loader.h>

namespace lyra {

namespace resource {

enum class MeshFlags : uint32 {
	calculateTangentSpace = 0x1,
	joinIdenticalVertices = 0x2,
	makeLeftHanded = 0x4,
	triangulate = 0x8,
	removeComponent = 0x10,
	generateNormals = 0x20,
	generateSmoothNormals = 0x40,
	splitLargeMeshes = 0x80,
	preTransformVertices = 0x100,
	limitBoneWeights = 0x200,
	validateDataStructure = 0x400,
	improveCacheLocality = 0x800,
	removeRedundantMaterials = 0x1000,
	fixInfacingNormals = 0x2000,
	populateArmatureData = 0x4000,
	sortByPType = 0x8000,
	findDegenerates = 0x10000,
	findInvalidData = 0x20000,
	generateUVCoords = 0x40000,
	transformUVCoords = 0x80000,
	findInstances = 0x100000,
	optimizeMeshes  = 0x200000,
	optimizeGraph  = 0x400000,
	flipUVs = 0x800000,
	flipWindingOrder  = 0x1000000,
	splitByBoneCount  = 0x2000000,
	debone  = 0x4000000,
	globalScale = 0x8000000,
	embedTextures  = 0x10000000,
	forceGenNormals = 0x20000000,
	dropNormals = 0x40000000,
	generateBoundingBoxes = 0x80000000
}; // Refer to of Assimp (aiPostProcessSteps) for the documentation of these enums

struct MeshFile {
	struct Vertex {
		glm::vec3 pos = glm::vec3(1.0f);
		glm::vec3 normal = glm::vec3(1.0f);
		glm::vec3 color = glm::vec3(1.0f);
		glm::vec3 uvw = glm::vec3(1.0f);
	};

	std::filesystem::path path;
	std::vector<uint32> indices;
	std::vector<Vertex> vertices;
};

NODISCARD MeshFile loadMesh(std::string_view path, MeshFlags flags = MeshFlags::generateSmoothNormals | MeshFlags::triangulate | MeshFlags::joinIdenticalVertices);

} // namespace resource

} // namespace lyra
