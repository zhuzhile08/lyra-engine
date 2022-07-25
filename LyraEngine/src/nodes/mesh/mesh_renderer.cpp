#include <components/mesh/mesh_renderer.h>

#include <components/mesh/mesh.h>

#include <components/graphics/camera.h>
#include <core/rendering/vulkan/GPU_buffer.h>

namespace lyra {

// mesh renderer
void MeshRenderer::create(const Mesh* const mesh, const Material* const material) {
	Logger::log_info("Creating Mesh Renderer... ");

	_mesh = mesh;
	_material = material;

	create_vertex_buffer();
	create_index_buffer();

	Logger::log_info("Successfully created MeshRenderer at ", get_address(this), "!", Logger::end_l());
}

void MeshRenderer::bind(Camera* const camera) noexcept {
	camera->add_to_draw_queue(FUNC_PTR(
		vkCmdBindPipeline(
			Application::context()->activeCommandBuffer(),
			_material->pipeline()->bindPoint(), _material->pipeline()->pipeline()
		);

		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(Application::context()->activeCommandBuffer(), 0, 1, &_vertexBuffer.buffer(), offsets);
		vkCmdBindIndexBuffer(Application::context()->activeCommandBuffer(), _indexBuffer.buffer(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(
			Application::context()->activeCommandBuffer(),
			_material->pipeline()->bindPoint(), _material->pipeline()->layout(), 0, 1, _material->descriptor()->get_ptr(), 0, nullptr
		);
		vkCmdDrawIndexed(Application::context()->activeCommandBuffer(), static_cast<uint32>(_mesh->indices().size()), 1, 0, 0, 0);
	));
}

void MeshRenderer::create_vertex_buffer() {
	// create the staging buffer
	VulkanGPUBuffer stagingBuffer;
	stagingBuffer.create(Application::context()->device(), sizeof(_mesh->vertices()[0]) * _mesh->vertices().size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	stagingBuffer.copy_data(_mesh->vertices().data());

	// create the vertex buffer
	_vertexBuffer.create(Application::context()->device(), sizeof(_mesh->vertices()[0]) * _mesh->vertices().size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	// copy the buffer
	_vertexBuffer.copy(Application::context()->commandPool(), &stagingBuffer);
}

void MeshRenderer::create_index_buffer() {
	// create the staging buffer
	VulkanGPUBuffer stagingBuffer;
	stagingBuffer.create(Application::context()->device(), sizeof(_mesh->indices()[0]) * _mesh->indices().size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	stagingBuffer.copy_data(_mesh->indices().data());

	// create the vertex buffer
	_indexBuffer.create(Application::context()->device(), sizeof(_mesh->indices()[0]) * _mesh->indices().size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	// copy the buffer
	_indexBuffer.copy(Application::context()->commandPool(), &stagingBuffer);
}

} // namespace lyra
