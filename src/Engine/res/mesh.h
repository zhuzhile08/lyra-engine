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
#include <noud.h>
#include <res/loaders/load_model.h>
#include <res/vertex.h>

#include <rendering/render_stage.h>
#include <rendering/renderer.h>
#include <rendering/vulkan/GPU_buffer.h>

#define GLM_FORCE_RADIANS

#include <vector>
#include <array>
#include <glm.hpp>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief A mesh struct containing vertices, indecies and vertex and index buffers
 */
class Mesh : noud::Node {
public:
	Mesh();

	/**
	 * @brief destroy the mesh
	 */
	void destroy() noexcept;

	/**
	 * @brief construct a new mesh loaded from a .obj file
	 *
	 * @param renderer the renderer
	 * @param path path of the model to load
	 * @index index of the object in the model to load. Starts at 1, 0 is default
	 * @param parent parent node
	 * @param name name of the node
	 */
	void create(Renderer renderer, const non_access::LoadedModel loaded, uint16 index = 0,
		noud::Node* parent = nullptr, const std::string name = "mesh");
	/**
	 * @brief construct a new mesh with a custom model
	 * @brief the vertices and indecies are user defined, which makes it perfect for generated meshes
	 *
	 * @param renderer the renderer
	 * @param vertices the new vertices
	 * @param indices the new indices
	 * @param parent parent node
	 * @param name name of the node
	 */
	void create(Renderer renderer, const std::vector <Vertex> vertices, const std::vector <uint16> indices,
		noud::Node* parent = nullptr, const std::string name = "mesh");

	/**
	 * @brief create a mesh from a already loaded .obj file
	 *
	 * @param load an already loaded model
	 * @param index load the model with the following index if a file has more than just one object. Will load everything on default
	 */
	void create_mesh(const non_access::LoadedModel loaded, const uint16 index = UINT16_MAX);

	/**
	 * add the mesh and its buffers to the renderer draw queue
	 *
	 * @param renderStage renderer to add the draw call to
	 */
	void draw(RenderStage renderStage) noexcept;

	/**
	 * @brief get the vertices
	 * 
	 * @return const std::vector <Vertex>
	*/
	const std::vector <Vertex> vertices() const noexcept { return _vertices; }
	/**
	 * @brief get the indices
	 *
	 * @return const std::vector <uint16>
	*/
	const std::vector <uint16> indices() const noexcept { return _indices; }
	/**
	 * @brief get the vertex buffer
	 * 
	 * @return const VulkanGPUBuffer
	*/
	const VulkanGPUBuffer vertexBuffer() const noexcept { return _vertexBuffer; }
	/**
	 * @brief get the index buffer
	 * 
	 * @return const VulkanGPUBuffer 
	*/
	const VulkanGPUBuffer indexBuffer() const noexcept { return _indexBuffer; }

private:
	std::vector <Vertex> _vertices;
	std::vector <uint16> _indices;

	VulkanGPUBuffer _vertexBuffer;
	VulkanGPUBuffer _indexBuffer;

	Renderer* renderer;

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
