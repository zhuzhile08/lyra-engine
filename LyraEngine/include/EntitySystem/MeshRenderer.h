/*************************
 * @file   MeshRenderer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief  a renderer for the mesh
 * 
 * @date   2022-19-7
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <Common/Common.h>

#include <vector>

#include <Graphics/VulkanImpl/GPUBuffer.h>

#include <EntitySystem/Entity.h>

namespace lyra {

/**
 * @brief a mesh renderer to render (multiple) meshes
 */
class MeshRenderer : public ComponentBase {
public:
	MeshRenderer() = default;
	/**
	 * @brief construct a mesh renderer
	 * 
	 * @param mesh mesh to render
	 * @param material material to render the mesh with
	 */
	MeshRenderer(const Mesh* const mesh, const Material* const material);

	DEFINE_DEFAULT_MOVE(MeshRenderer)

	/**
	 * @brief get the vertex buffer
	 *
	 * @return const vulkan::GPUBuffer&
	*/
	NODISCARD constexpr const vulkan::GPUBuffer& vertexBuffer() const noexcept { return m_vertexBuffer; }
	/**
	 * @brief get the index buffer
	 *
	 * @return const vulkan::GPUBuffer&
	*/
	NODISCARD constexpr const vulkan::GPUBuffer& indexBuffer() const noexcept { return m_indexBuffer; }

private:
	const Mesh* m_mesh;
	const Material* m_material;

	vulkan::GPUBuffer m_vertexBuffer;
	vulkan::GPUBuffer m_indexBuffer;

	std::vector<MeshRenderer*>::iterator m_iterator;

	void update() { };
	
	/**
	 * bind the buffers of the mesh and draw it
	 */
	void draw() const noexcept;

	friend class Material;
	friend class CubemapBase;
};

} // namespace lyra
