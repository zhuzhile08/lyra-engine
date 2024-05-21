#include <ECS/Components/MeshRenderer.h>

namespace lyra {

// mesh renderer
MeshRenderer::MeshRenderer(const Mesh& mesh, Material& material
) : m_mesh(&mesh),
	m_material(&material),
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

} // namespace lyra
