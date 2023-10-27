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
#include <Common/RenderSystem.h>

#include <EntitySystem/Entity.h>

#include <Graphics/VulkanRenderSystem.h>

#include <vector>

namespace lyra {

class MeshRenderer : public ComponentBase, public RenderObject {
public:
	MeshRenderer() = default;
	MeshRenderer(const Mesh* const mesh, const Material* const material);

	DEFINE_DEFAULT_MOVE(MeshRenderer)

	NODISCARD constexpr const vulkan::GPUBuffer& vertexBuffer() const noexcept { return m_vertexBuffer; }
	NODISCARD constexpr const vulkan::GPUBuffer& indexBuffer() const noexcept { return m_indexBuffer; }

private:
	const Mesh* m_mesh;
	const Material* m_material;

	vulkan::GPUBuffer m_vertexBuffer;
	vulkan::GPUBuffer m_indexBuffer;

	void update() { };
	
	/**
	 * bind the buffers of the mesh and draw it
	 */
	void draw() const noexcept;

	friend class Material;
	friend class CubemapBase;
};

} // namespace lyra
