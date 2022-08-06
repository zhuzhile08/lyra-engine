/*************************
 * @file   mesh_renderer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief  a renderer for the mesh
 * 
 * @date   2022-19-7
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <core/decl.h>
#include <nodes/node.h>
#include <core/rendering/material.h>

#include <vector>
#include <glm.hpp>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief A mesh struct containing vertices, indecies and vertex and index buffers
 */
class MeshRenderer : public Node<Spatial> {
public:
	MeshRenderer() { }

	/**
	 * @brief construct a new mesh loaded from a .obj file
	 *
	 * @param mesh mesh
	 * @param material material of the mesh
	 */
	MeshRenderer(
		const Mesh* const mesh, 
		const Material* const material,
		const char* name = "MeshRenderer",
		Spatial* parent = nullptr,
		const bool visible = true,
		const uint32 tag = 0
	);

	MeshRenderer operator=(const MeshRenderer&) const noexcept = delete;

	/**
	 * add the mesh and its buffers to the renderer draw queue
	 *
	 * @param camera context to add the draw call to
	 */
	void bind(Camera* const camera) noexcept;

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
	/**
	 * @brief get the material
	 *
	 * @return const lyra::Material* const
	*/
	[[nodiscard]] const Material* const material() const noexcept { return _material; }

private:
	const Material* _material;
	const Mesh* _mesh;

	VulkanGPUBuffer _vertexBuffer;
	VulkanGPUBuffer _indexBuffer;

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
