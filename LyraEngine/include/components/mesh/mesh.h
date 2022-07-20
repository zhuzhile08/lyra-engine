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

#include <core/defines.h>
#include <res/loaders/load_model.h>
#include <components/graphics/camera.h>
#include <core/rendering/vulkan/GPU_buffer.h>

#include <vector>
#include <glm.hpp>
#include <vulkan/vulkan.h>

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
		[[nodiscard]] static const VkVertexInputBindingDescription get_binding_description() noexcept;

		/**
		 * @brief returns a static vertex input attribute
		 *
		 * @return std::array<VkVertexInputAttributeDescription, 4>
		 */
		[[nodiscard]] static const std::array<VkVertexInputAttributeDescription, 4> get_attribute_descriptions() noexcept;
	};

	Mesh() { }

	Mesh operator=(const Mesh&) const noexcept = delete;

	/**
	 * @brief construct a new mesh loaded from a .obj file
	 *
	 * @param path path of the model
	 * @param index index of the object in the model to load. Starts at 1, 0 is default
	 */
	void create(const char* path, const uint16 index = 0);
	/**
	 * @brief construct a new mesh with a custom model
	 * @brief the vertices and indecies are user defined, which makes it perfect for generated meshes
	 *
	 * @param vertices the new vertices
	 * @param indices the new indices
	 */
	void create(const std::vector <Vertex> vertices, const std::vector <uint32> indices);

	/**
	 * @brief get the vertices
	 * 
	 * @return const std::vector <lyra::Vertex>
	*/
	[[nodiscard]] const std::vector <Vertex> vertices() const noexcept { return _vertices; }
	/**
	 * @brief get the indices
	 *
	 * @return const std::vector <uint16>
	*/
	[[nodiscard]] const std::vector <uint32> indices() const noexcept { return _indices; }

private:
	std::vector <Vertex> _vertices;
	std::vector <uint32> _indices;

	/**
	 * @brief create a mesh from a already loaded .obj file
	 *
	 * @param load an already loaded model
	 * @param index load the model with the following index if a file has more than just one object. Will load everything on default
	 */
	void create_mesh(const non_access::LoadedModel loaded, const uint16 index = UINT16_MAX);
};

} // namespace lyra
