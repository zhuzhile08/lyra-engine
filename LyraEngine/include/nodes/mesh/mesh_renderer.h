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

#include <core/smart_pointer.h>

#include <nodes/node.h>

namespace lyra {

/**
 * @brief a mesh renderer to render (multiple) meshes
 */
class MeshRenderer : public Spatial {
public:
	/**
	 * @brief construct a mesh renderer
	 * 
	 * @param mesh mesh
	 * @param name name of the object
	 * @param parent parent of the object
	 * @param script script of the object
	 * @param tag optional tag of the object
	 */
	MeshRenderer(
		const Mesh* const mesh, 
		const char* name = "MeshRenderer",
		Spatial* parent = nullptr,
		Script<MeshRenderer>* script = new Script<MeshRenderer>,
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

	LYRA_NODE_SCRIPT_MEMBER(MeshRenderer)

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
