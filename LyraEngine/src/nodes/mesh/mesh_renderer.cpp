#include <nodes/mesh/mesh_renderer.h>

#include <nodes/mesh/mesh.h>

#include <rendering/vulkan/command_buffer.h>
#include <rendering/vulkan/GPU_buffer.h>

#include <core/application.h>

namespace lyra {

// mesh renderer
MeshRenderer::MeshRenderer(
	const Mesh* const mesh, 
	Script* script,
	const char* name,
	Spatial* parent,
	const uint32& tag
) :
	Spatial(nullptr, name, parent, true, tag), m_mesh(mesh)
{
	Logger::log_info("Creating Mesh Renderer... ");

	// create the vertex and index buffer
	m_vertexBuffer = SmartPointer<vulkan::GPUBuffer>::create(sizeof(m_mesh->vertices()[0]) * m_mesh->vertices().size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	m_indexBuffer = SmartPointer<vulkan::GPUBuffer>::create(sizeof(m_mesh->indices()[0]) * m_mesh->indices().size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	create_vertex_buffer();
	create_index_buffer();

	Logger::log_info("Successfully created MeshRenderer at ", get_address(this), "!", Logger::end_l());
}

void MeshRenderer::create_vertex_buffer() {
	// create the staging buffer
	vulkan::GPUBuffer stagingBuffer(sizeof(m_mesh->vertices()[0]) * m_mesh->vertices().size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	stagingBuffer.copy_data(m_mesh->vertices().data());

	// copy the buffer
	m_vertexBuffer->copy(&stagingBuffer);
}

void MeshRenderer::create_index_buffer() {
	// create the staging buffer
	vulkan::GPUBuffer stagingBuffer(sizeof(m_mesh->indices()[0]) * m_mesh->indices().size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	
	stagingBuffer.copy_data(m_mesh->indices().data());

	// copy the buffer
	m_indexBuffer->copy(&stagingBuffer);
}

void MeshRenderer::draw() const noexcept {
	// bind index and vertex buffer
	Application::renderSystem()->currentCommandBuffer().bindVertexBuffer(0, 1, m_vertexBuffer->buffer(), 0);
	Application::renderSystem()->currentCommandBuffer().bindIndexBuffer(m_indexBuffer->buffer(), 0, VK_INDEX_TYPE_UINT32);
	// draw
	Application::renderSystem()->currentCommandBuffer().drawIndexed(static_cast<uint32>(m_mesh->indices().size()), 1, 0, 0, 0);
}

} // namespace lyra
