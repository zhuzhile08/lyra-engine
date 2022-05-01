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
#include <core/rendering/vulkan/descriptor.h>
#include <res/loaders/load_model.h>
#include <core/rendering/vulkan/vertex.h>
#include <core/rendering/vulkan/GPU_buffer.h>
#include <graphics/renderer.h>
#include <graphics/texture.h>
#include <graphics/camera.h>

#include <noud.h>

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
	 * @brief destructor of the mesh
	 */
	virtual ~Mesh() noexcept;

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
	 * @param parent parent node
	 * @param name name of the node
	 */
	void create(const string path, const uint16 index = 0, const noud::Node* const parent = nullptr, const string name = "mesh");
	/**
	 * @brief construct a new mesh with a custom model
	 * @brief the vertices and indecies are user defined, which makes it perfect for generated meshes
	 *
	 * @param vertices the new vertices
	 * @param indices the new indices
	 * @param parent parent node
	 * @param name name of the node
	 */
	void create(const std::vector <Vertex> vertices, const std::vector <uint16> indices, const noud::Node* const parent = nullptr, const string name = "mesh");

	/**
	 * @brief bind a texture to the model
	 * 
	 * @param texture the texture to bind
	*/
	void bind_texture(const Texture* const texture);
	/**
	 * @brief bind a camera to the model
	 * 
	 * @param camera the camera to bind
	*/
	void bind_camera(const Camera* const camera);

	/**
	 * add the mesh and its buffers to the renderer draw queue
	 *
	 * @param renderer context to add the draw call to
	 */
	void bind(Renderer* const renderer) noexcept;

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
	[[nodiscard]] const std::vector <uint16> indices() const noexcept { return _indices; }
	/**
	 * @brief get the vertex buffer
	 * 
	 * @return const lyra::VulkanGPUBuffer
	*/
	[[nodiscard]] const VulkanGPUBuffer& vertexBuffer() const noexcept { return _vertexBuffer; }
	/**
	 * @brief get the index buffer
	 * 
	 * @return const lyra::VulkanGPUBuffer 
	*/
	[[nodiscard]] const VulkanGPUBuffer& indexBuffer() const noexcept { return _indexBuffer; }

private:
	std::vector <Vertex> _vertices;
	std::vector <uint16> _indices;

	VulkanGPUBuffer _vertexBuffer;
	VulkanGPUBuffer _indexBuffer;
	VulkanDescriptor _descriptor;

	VulkanDescriptor::Writer _writer;

	const Context* context;

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
