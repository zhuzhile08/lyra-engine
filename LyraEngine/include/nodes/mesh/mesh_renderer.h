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

#include <memory>
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
		const char* name = "MeshRenderer",
		Spatial* parent = nullptr,
		const bool visible = true,
		const uint32 tag = 0
	);

	MeshRenderer operator=(const MeshRenderer&) const noexcept = delete;

	/**
	 * @brief get the vertex buffer
	 *
	 * @return const vulkan::GPUBuffer>
	*/
	NODISCARD const vulkan::GPUBuffer* const vertexBuffer() const noexcept { return m_vertexBuffer; }
	/**
	 * @brief get the index buffer
	 *
	 * @return const vulkan::GPUBuffer>
	*/
	NODISCARD const vulkan::GPUBuffer* const indexBuffer() const noexcept { return m_indexBuffer; }

private:
	const Mesh* m_mesh;

	SmartPointer<vulkan::GPUBuffer> m_vertexBuffer;
	SmartPointer<vulkan::GPUBuffer> m_indexBuffer;

	/**
	 * @brief create a vertex buffer
	 */
	void create_vertex_buffer();
	/**
	 * @brief create a index buffer
	 */
	void create_index_buffer();

	/**
	 * bind the buffers of the mesh and draw it
	 */
	void draw() const noexcept;

	friend class Material;
};

} // namespace lyra
