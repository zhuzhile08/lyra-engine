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

#include <Lyra/Lyra.h>

#include <Graphics/VulkanImpl/GPUBuffer.h>

#include <EntitySystem/Entity.h>

namespace lyra {

/**
 * @brief a mesh renderer to render (multiple) meshes
 * 
 * @tparam _STy script type
 */
class MeshRenderer : public ComponentBase {
public:
	MeshRenderer() = default;
	/**
	 * @brief construct a mesh renderer
	 * 
	 * @tparam MSTy script type for the mesh
	 * 
	 * @param mesh mesh
	 */
	MeshRenderer(const Mesh* const mesh);

	DEFINE_DEFAULT_MOVE(MeshRenderer)

	/**
	 * @brief get the vertex buffer
	 *
	 * @return constexpr const vulkan::GPUBuffer&
	*/
	NODISCARD constexpr const vulkan::GPUBuffer& vertexBuffer() const noexcept { return m_vertexBuffer; }
	/**
	 * @brief get the index buffer
	 *
	 * @return constexpr const vulkan::GPUBuffer&
	*/
	NODISCARD constexpr const vulkan::GPUBuffer& indexBuffer() const noexcept { return m_indexBuffer; }

private:
	const Mesh* m_mesh;

	vulkan::GPUBuffer m_vertexBuffer;
	vulkan::GPUBuffer m_indexBuffer;

	/**
	 * bind the buffers of the mesh and draw it
	 */
	void update() const noexcept;

	friend class Material;
	friend class CubemapBase;
};

} // namespace lyra
