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
#include <core/rendering/vulkan/vertex.h>
#include <components/graphics/camera.h>
#include <core/rendering/vulkan/GPU_buffer.h>
#include <graphics/renderer.h>

#include <vector>
#include <glm.hpp>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief A mesh struct containing vertices, indecies and vertex and index buffers
 */
class Mesh {
public:
	Mesh();

	/**
	 * @brief destroy the mesh
	 */
	void destroy() noexcept;

	Mesh operator=(const Mesh&) const noexcept = delete;

	/**
	 * @brief construct a new mesh loaded from a .obj file
	 *
	 * @param path path of the model
	 * @index index of the object in the model to load. Starts at 1, 0 is default
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
	 * add the mesh and its buffers to the renderer draw queue
	 *
	 * @param camera context to add the draw call to
	 */
	void bind(Camera* const camera) noexcept;

	/**
	 * @brief get the vertices
	 * 
	 * @return const std::vector <Vertex>
	*/
	[[nodiscard]] const std::vector <Vertex> vertices() const noexcept { return _vertices; }
	/**
	 * @brief get the indices
	 *
	 * @return const std::vector <uint16>
	*/
	[[nodiscard]] const std::vector <uint32> indices() const noexcept { return _indices; }
	/**
	 * @brief get the vertex buffer
	 * 
	 * @return const lyra::VulkanGPUBuffer* const
	*/
	[[nodiscard]] const VulkanGPUBuffer* const vertexBuffer() const noexcept { return &_vertexBuffer; }
	/**
	 * @brief get the index buffer
	 * 
	 * @return const lyra::VulkanGPUBuffer* const
	*/
	[[nodiscard]] const VulkanGPUBuffer* const indexBuffer() const noexcept { return &_indexBuffer; }

private:
	std::vector <Vertex> _vertices;
	std::vector <uint32> _indices;

	VulkanGPUBuffer _vertexBuffer;
	VulkanGPUBuffer _indexBuffer;

	/**
	 * @brief create a mesh from a already loaded .obj file
	 *
	 * @param load an already loaded model
	 * @param index load the model with the following index if a file has more than just one object. Will load everything on default
	 */
	void create_mesh(const non_access::LoadedModel loaded, const uint16 index = UINT16_MAX);

	/**
	 * @brief create a vertex buffer
	 */
	void create_vertex_buffer();
	/**
	 * @brief create a index buffer
	 */
	void create_index_buffer();
};

} // namespace lyra
