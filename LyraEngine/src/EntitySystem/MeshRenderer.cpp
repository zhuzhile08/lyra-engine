#include <EntitySystem/MeshRenderer.h>

#include <Resource/Mesh.h>

#include <Graphics/VulkanImpl/CommandBuffer.h>
#include <Graphics/VulkanImpl/GPUBuffer.h>

#include <Application/Application.h>

namespace lyra {

// mesh renderer
MeshRenderer::MeshRenderer(const Mesh* const mesh, const Material* const material
) : m_mesh(mesh),
	m_material(material),
	m_vertexBuffer(sizeof(m_mesh->vertices()[0]) * m_mesh->vertices().size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU),
	m_indexBuffer(sizeof(m_mesh->indices()[0]) * m_mesh->indices().size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU)
{
	// create the staging buffer for the vertex buffer
	vulkan::GPUBuffer vertexStagingBuffer(sizeof(m_mesh->vertices()[0]) * m_mesh->vertices().size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	vertexStagingBuffer.copyData(m_mesh->vertices().data());
	// copy the buffer
	m_vertexBuffer.copy(vertexStagingBuffer);

	// create the staging buffer for the index buffer
	vulkan::GPUBuffer indexStagingBuffer(sizeof(m_mesh->indices()[0]) * m_mesh->indices().size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	indexStagingBuffer.copyData(m_mesh->indices().data());
	// copy the buffer
	m_indexBuffer.copy(indexStagingBuffer);
}

void MeshRenderer::draw() const noexcept {
	// bind index and vertex buffer
	Application::renderSystem.frames[Application::renderSystem.currentFrame()].commandBuffer().bindVertexBuffer(0, 1, m_vertexBuffer.buffer(), 0);
	Application::renderSystem.frames[Application::renderSystem.currentFrame()].commandBuffer().bindIndexBuffer(m_indexBuffer.buffer(), 0, VK_INDEX_TYPE_UINT32);
	// draw
	Application::renderSystem.frames[Application::renderSystem.currentFrame()].commandBuffer().drawIndexed(static_cast<uint32>(m_mesh->indices().size()), 1, 0, 0, 0);
}

} // namespace lyra
