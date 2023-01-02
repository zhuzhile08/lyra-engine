/*************************
 * @file mesh.h
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

#include <lyra.h>

#include <vector>
#include <core/array.h>

#include <glm.hpp>

#include <vulkan/vulkan.h>

#include <res/loaders/load_model.h>

#include <nodes/node.h>
#include <nodes/spatial.h>

namespace lyra {

/**
 * @brief A mesh struct containing vertices, indecies and vertex and index buffers
 */
class  Mesh : public Spatial {
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
		 * @return VkVertexInputBindingDescription
		 */
		NODISCARD static const VkVertexInputBindingDescription get_binding_description() noexcept;

		/**
		 * @brief returns a static vertex input attribute
		 *
		 * @return Array<VkVertexInputAttributeDescription, 4>
		 */
		NODISCARD static const Array<VkVertexInputAttributeDescription, 4> get_attribute_descriptions() noexcept;
	};

	/**
	 * @brief construct a new mesh loaded from a .obj file
	 *
	 * @param path path of the model
	 * @param script script of the object
	 * @param name name of the object
	 * @param parent parent Node of the object
	 * @param visible visibility of the object
	 * @param tag optional tag of the object
	 * @param transform transform of the object
	 */
	Mesh(
		const char* path, 
		Script* script,
		const char* name = "Mesh",
		Spatial* parent = nullptr,
		const bool& visible = true,
		const uint32& tag = 0,
		const Transform& transform = Transform()
	);

	/**
	 * @brief construct a new mesh with a custom ´mesh
	 *
	 * @param vertices the new vertices
	 * @param indices the new indices
	 * @param script script of the object
	 * @param name name of the object
	 * @param parent parent Node of the object
	 * @param visible visibility of the object
	 * @param tag optional tag of the object
	 * @param transform transform of the object
	 */
	Mesh(
		const std::vector <Vertex>& vertices, 
		const std::vector <uint32>& indices, 
		Script* script,
		const char* name = "Mesh",
		Spatial* parent = nullptr,
		const bool& visible = true,
		const uint32& tag = 0,
		const Transform& transform = Transform()
	);

	Mesh operator=(const Mesh&) const noexcept = delete;

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
