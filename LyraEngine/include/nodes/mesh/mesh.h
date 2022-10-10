/*************************
 * @file mesh.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a file with geometry and mesh related structs
 * @brief has a struct for vertexes, indexes, uniform buffers and meshes
 *
 * @date 2022-02-03
 *
 * @copyright Copyright (c) 2022 Zhile Zhu
 *************************/

#pragma once

#include <vector>
#include <array>

#include <glm.hpp>

#include <vulkan/vulkan.h>

#include <core/decl.h>
#include <core/util.h>

#include <res/loaders/load_model.h>

#include <nodes/node.h>
#include <nodes/spatial.h>

namespace lyra {

/**
 * @brief A mesh struct containing vertices, indecies and vertex and index buffers
 */
class Mesh : public Spatial {
public:
	// vertex
	struct Vertex {
		glm::vec3 pos = glm::vec3(1.0f);
		glm::vec3 normal = glm::vec3(1.0f);
		glm::vec3 color = glm::vec3(1.0f);
		glm::vec2 uv = glm::vec2(1.0f);

		Vertex() { }

		/**
		 * @brief construct a new Vertex object
		 *
		 * @param pos the new position
		 * @param normal vertex normals
		 * @param color the new color
		 */
		Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 uv, glm::vec3 color = { 0, 0, 0 }) : pos(pos), normal(normal), color(color), uv(uv) { }

		/**
		 * @brief returns a static vertex binding
		 *
		 * @return VkVertexInputBindingDescription
		 */
		NODISCARD static const VkVertexInputBindingDescription get_binding_description() noexcept;

		/**
		 * @brief returns a static vertex input attribute
		 *
		 * @return std::array<VkVertexInputAttributeDescription, 4>
		 */
		NODISCARD static const std::array<VkVertexInputAttributeDescription, 4> get_attribute_descriptions() noexcept;
	};

	/**
	 * @brief construct a new mesh loaded from a .obj file
	 *
	 * @param path path of the model
	 * @param index index of the object in the model to load. Starts at 1, 0 is default
	 * @param name name of the object
	 * @param parent parent Node of the object
	 * @param script script of the object
	 * @param visible visibility of the object
	 * @param tag optional tag of the object
	 * @param position position of the object
	 * @param rotation rotation of the object
	 * @param scale scale of the object
	 * @param rotationOrder order of the multiplication of the rotation matricies
	 */
	Mesh(
		const char* path, 
		const uint16 index = 0,
		const char* name = "Mesh",
		Spatial* parent = nullptr,
		Script<Mesh>* script = new Script<Mesh>,
		const bool visible = true,
		const uint32 tag = 0,
		const glm::vec3 position = { 0.0f, 0.0f, 0.0f },
		const glm::vec3 rotation = { 0.0f, 0.0f, 0.0f },
		const glm::vec3 scale = { 1.0f, 1.0f, 1.0f },
		const RotationOrder rotationOrder = RotationOrder::ROTATION_ZYX
	);

	/**
	 * @brief construct a new mesh with a custom ´mesh
	 *
	 * @param vertices the new vertices
	 * @param indices the new indices
	 * @param name name of the object
	 * @param parent parent Node of the object
	 * @param script script of the object
	 * @param visible visibility of the object
	 * @param tag optional tag of the object
	 * @param position position of the object
	 * @param rotation rotation of the object
	 * @param scale scale of the object
	 * @param rotationOrder order of the multiplication of the rotation matricies
	 */
	Mesh(
		const std::vector <Vertex> vertices, 
		const std::vector <uint32> indices, 
		const char* name = "Mesh",
		Spatial* parent = nullptr,
		Script<Mesh>* script = new Script<Mesh>,
		const bool visible = true,
		const uint32 tag = 0,
		const glm::vec3 position = { 0.0f, 0.0f, 0.0f },
		const glm::vec3 rotation = { 0.0f, 0.0f, 0.0f },
		const glm::vec3 scale = { 1.0f, 1.0f, 1.0f },
		const RotationOrder rotationOrder = RotationOrder::ROTATION_ZYX
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

	LYRA_NODE_SCRIPT_MEMBER(Mesh)

	/**
	 * @brief create a mesh from a already loaded .obj file
	 *
	 * @param load an already loaded model
	 * @param index load the model with the following index if a file has more than just one object. Will load everything on default
	 */
	void create_mesh(const util::LoadedModel& loaded, const uint16 index = UINT16_MAX);
};

} // namespace lyra
