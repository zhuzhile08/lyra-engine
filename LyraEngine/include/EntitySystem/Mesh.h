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

#include <Lyra/Lyra.h>

#include <vector>
#include <Common/Array.h>

#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

#include <Resource/LoadModel.h>

#include <EntitySystem/Entity.h>

namespace lyra {

/**
 * @brief A mesh struct containing vertices, indecies and vertex and index buffers
 */
class Mesh {
public:
	// vertex
	struct Vertex {
		glm::vec3 pos = glm::vec3(1.0f);
		glm::vec3 normal = glm::vec3(1.0f);
		glm::vec3 color = glm::vec3(1.0f);
		glm::vec3 uvw = glm::vec3(1.0f);

		Vertex() { }

		/**
		 * @brief construct a new Vertex object
		 *
		 * @param pos the new position
		 * @param normal vertex normals
		 * @param color the new color
		 */
		Vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec3& uvw, const glm::vec3& color = { 0, 0, 0 }) : pos(pos), normal(normal), color(color), uvw(uvw) { }

		/**
		 * @brief returns a static vertex binding
		 *
		 * @return const VkVertexInputBindingDescription
		 */
		NODISCARD static const VkVertexInputBindingDescription get_binding_description() noexcept {
			return {
				0,
				sizeof(Vertex),
				VK_VERTEX_INPUT_RATE_VERTEX
			};
		}

		/**
		 * @brief returns a static vertex input attribute
		 *
		 * @return const Array<VkVertexInputAttributeDescription, 4>
		 */
		NODISCARD static const Array<VkVertexInputAttributeDescription, 4> get_attribute_descriptions() noexcept {
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

	Mesh() = default;
	/**
	 * @brief construct a new mesh loaded from a .obj file
	 *
	 * @param path path of the model
	 */
	Mesh(
		std::string_view path
	);

	/**
	 * @brief construct a new mesh with a custom ´mesh
	 *
	 * @param vertices the new vertices
	 * @param indices the new indices
	 */
	Mesh(
		const std::vector <Vertex>& vertices, 
		const std::vector <uint32>& indices
	);

	/**
	 * @brief get the vertices
	 * 
	 * @return const std::vector <lyra::Vertex>
	*/
	NODISCARD const std::vector <Vertex> vertices() const noexcept { return m_vertices; }
	/**
	 * @brief get the indices
	 *
	 * @return const std::vector <uint16>
	*/
	NODISCARD const std::vector <uint32> indices() const noexcept { return m_indices; }

private:
	std::vector <Vertex> m_vertices;
	std::vector <uint32> m_indices;

	/**
	 * @brief create a mesh from a already loaded .obj file
	 *
	 * @param load an already loaded model
	 */
	void create_mesh(const util::LoadedModel& loaded);
};

} // namespace lyra