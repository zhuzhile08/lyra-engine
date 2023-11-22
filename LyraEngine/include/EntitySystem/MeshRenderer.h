/*************************
 * @file MeshRenderer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief a renderer for the mesh
 * 
 * @date 2022-19-7
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <Common/Common.h>
#include <Common/RenderSystem.h>

#include <EntitySystem/Entity.h>

#include <Graphics/Renderer.h>
#include <Graphics/VulkanRenderSystem.h>
#include <Graphics/Mesh.h>
#include <Graphics/Material.h>

#include <vector>

namespace lyra {

class MeshRenderer : public ComponentBase {
public:
	MeshRenderer() = default;
	MeshRenderer(const Mesh& mesh, Material& material);

	NODISCARD constexpr const Material* material() const noexcept {
		return m_material;
	}
	NODISCARD constexpr Material* material() noexcept {
		return m_material;
	}
	NODISCARD constexpr const Mesh* mesh() const noexcept {
		return m_mesh;
	}
	NODISCARD constexpr const vulkan::GPUBuffer& vertexBuffer() const noexcept {
		return m_vertexBuffer;
	}
	NODISCARD constexpr const vulkan::GPUBuffer& indexBuffer() const noexcept {
		return m_indexBuffer;
	}

private:
	const Mesh* m_mesh;
	Material* m_material;

	vulkan::GPUBuffer m_vertexBuffer;
	vulkan::GPUBuffer m_indexBuffer;

	void update() { };

	friend void renderer::draw();
	friend void renderer::setScene(Entity&);
};

} // namespace lyra
