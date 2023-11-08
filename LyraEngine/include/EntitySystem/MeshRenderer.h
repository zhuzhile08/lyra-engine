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

#include <Graphics/Renderer.h>
#include <Graphics/VulkanRenderSystem.h>

#include <vector>

namespace lyra {

class MeshRenderer : public ComponentBase {
public:
	MeshRenderer() = default;
	MeshRenderer(const Mesh* const mesh, const Material* const material);

private:
	const Mesh* m_mesh;
	const Material* m_material;

	vulkan::GPUBuffer m_vertexBuffer;
	vulkan::GPUBuffer m_indexBuffer;

	void update() { };

	friend class Material;
	friend class renderSystem::Renderer;
};

} // namespace lyra
