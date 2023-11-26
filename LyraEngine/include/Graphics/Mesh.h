/*************************
 * @file Mesh.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a file with geometry and mesh related structs
 * @brief has a struct for vertexes, indexes, uniform buffers and meshes
 *
 * @date 2022-02-03
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/Array.h>

#include <Resource/LoadMeshFile.h>

#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

#include <vector>

namespace lyra {

class Mesh {
public:
	struct Vertex {
		glm::vec3 pos = glm::vec3(1.0f);
		glm::vec3 normal = glm::vec3(1.0f);
		glm::vec3 color = glm::vec3(1.0f);
		glm::vec3 uvw = glm::vec3(1.0f);

		constexpr Vertex() = default;
		constexpr Vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec3& color, const glm::vec3& uvw) : pos(pos), normal(normal), color(color), uvw(uvw) { }

		NODISCARD static constexpr VkVertexInputBindingDescription bindingDescription() noexcept {
			return {
				0,
				sizeof(Vertex),
				VK_VERTEX_INPUT_RATE_VERTEX
			};
		}

		NODISCARD static constexpr Array<VkVertexInputAttributeDescription, 4> attributeDescriptions() noexcept {
			return {
				{{
					0,
					0,
					VK_FORMAT_R32G32B32_SFLOAT,
					offsetof(Vertex, pos)
				},
				{
					1,
					0,
					VK_FORMAT_R32G32B32_SFLOAT,
					offsetof(Vertex, normal)
				},
				{
					2,
					0,
					VK_FORMAT_R32G32B32_SFLOAT,
					offsetof(Vertex, color)
				},
				{
					3,
					0,
					VK_FORMAT_R32G32B32_SFLOAT,
					offsetof(Vertex, uvw)
				}}
			};
		}
	};

	constexpr Mesh() = default;
#ifndef _WIN32
	constexpr
#endif
	Mesh(const resource::MeshFile& mesh, uint32 index) : 
		m_vertices(mesh.vertexBlocks[index]), 
		m_indices(mesh.indexData[index]) { 
		std::memcpy(
			m_vertices.data(),
			mesh.vertexData[index].data(), 
			mesh.vertexData[index].size()
		);
	}

	Mesh(
		const std::vector <Vertex>& vertices, 
		const std::vector <uint32>& indices
	) : m_vertices(vertices), m_indices(indices) { }

	NODISCARD std::vector <Vertex> vertices() const noexcept { return m_vertices; }
	NODISCARD std::vector <uint32> indices() const noexcept { return m_indices; }

private:
	std::vector <Vertex> m_vertices;
	std::vector <uint32> m_indices;
};

} // namespace lyra
