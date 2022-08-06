#include <nodes/mesh/mesh_renderer.h>

#include <nodes/mesh/mesh.h>

#include <nodes/graphics/camera.h>
#include <core/rendering/graphics_pipeline.h>
#include <core/rendering/vulkan/GPU_buffer.h>
#include <core/rendering/vulkan/descriptor.h>

namespace lyra {

// mesh renderer
MeshRenderer::MeshRenderer(
	const Mesh* const mesh, 
	const Material* const material, 
	const char* name,
	Spatial* parent,
	const bool visible,
	const uint32 tag
) :
	_mesh(mesh),
	_material(material),
	_vertexBuffer(Context::get()->renderSystem()->device().get(), sizeof(_mesh->vertices()[0])* _mesh->vertices().size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU),
	_indexBuffer(Context::get()->renderSystem()->device().get(), sizeof(_mesh->indices()[0])* _mesh->indices().size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU)
{
	Logger::log_info("Creating Mesh Renderer... ");

	create_vertex_buffer();
	create_index_buffer();

	Logger::log_info("Successfully created MeshRenderer at ", get_address(this), "!", Logger::end_l());
}

void MeshRenderer::bind(Camera* const camera) noexcept {
	camera->add_to_draw_queue(FUNC_PTR(
		vkCmdBindPipeline(
			Context::get()->renderSystem()->activeCommandBuffer(),
			_material->graphicsPipeline()->bindPoint(), _material->graphicsPipeline()->pipeline()
		);
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(Context::get()->renderSystem()->activeCommandBuffer(), 0, 1, &_vertexBuffer.buffer(), offsets);
		vkCmdBindIndexBuffer(Context::get()->renderSystem()->activeCommandBuffer(), _indexBuffer.buffer(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(
			Context::get()->renderSystem()->activeCommandBuffer(),
			_material->graphicsPipeline()->bindPoint(), _material->graphicsPipeline()->layout(), 0, 1, _material->descriptor()->get_ptr(), 0, nullptr
		);
		vkCmdDrawIndexed(Context::get()->renderSystem()->activeCommandBuffer(), static_cast<uint32>(_mesh->indices().size()), 1, 0, 0, 0);
	));
}

void MeshRenderer::create_vertex_buffer() {
	// create the staging buffer
	VulkanGPUBuffer stagingBuffer(Context::get()->renderSystem()->device().get(), sizeof(_mesh->vertices()[0]) * _mesh->vertices().size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	stagingBuffer.copy_data(_mesh->vertices().data());

	// copy the buffer
	_vertexBuffer.copy(Context::get()->renderSystem()->commandPool().get(), &stagingBuffer);
}

void MeshRenderer::create_index_buffer() {
	// create the staging buffer
	VulkanGPUBuffer stagingBuffer(Context::get()->renderSystem()->device().get(), sizeof(_mesh->indices()[0]) * _mesh->indices().size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	
	stagingBuffer.copy_data(_mesh->indices().data());

	// copy the buffer
	_indexBuffer.copy(Context::get()->renderSystem()->commandPool().get(), &stagingBuffer);
}

} // namespace lyra
