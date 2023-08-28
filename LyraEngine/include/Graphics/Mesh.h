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

#include <vector>
#include <Common/Array.h>

#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

// #include <Resource/LoadMesh.h>

namespace lyra {

class Mesh {
public:
	struct Vertex {
		glm::vec3 pos = glm::vec3(1.0f);
		glm::vec3 normal = glm::vec3(1.0f);
		glm::vec3 color = glm::vec3(1.0f);
		glm::vec3 uvw = glm::vec3(1.0f);

		constexpr Vertex() = default;
		constexpr Vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec3& uvw, const glm::vec3& color = { 0, 0, 0 }) : pos(pos), normal(normal), color(color), uvw(uvw) { }
		// constexpr Vertex(resource::MeshFile::Vertex vertex) : pos(vertex.pos), normal(vertex.normal), color(vertex.color), uvw(vertex.uvw)  { }

		NODISCARD static constexpr VkVertexInputBindingDescription getBindingDescription() noexcept {
			return {
				0,
				sizeof(Vertex),
				VK_VERTEX_INPUT_RATE_VERTEX
			};
		}

		NODISCARD static constexpr Array<VkVertexInputAttributeDescription, 4> getAttribute_descriptions() noexcept {
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
	// constexpr Mesh(const resource::MeshFile& mesh) : m_vertices(mesh.vertices.begin(), mesh.vertices.end()), m_indices(mesh.indices) { }

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
