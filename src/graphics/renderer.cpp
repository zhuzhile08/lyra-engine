#include <graphics/renderer.h>

namespace lyra {

Renderer::Renderer() {
	Logger::log_info("Creating Renderer...");

	_framebuffers.create(Application::context()->device(), Application::context()->swapchain());
	_pipeline.create(
		{
			&_framebuffers,
			{ { VulkanShader::Type::TYPE_VERTEX, "data/shader/vert.spv", "main" }, { VulkanShader::Type::TYPE_FRAGMENT, "data/shader/frag.spv", "main" } },
			Application::context()->swapchain()->extent(),
			Application::context()->swapchain()->extent()
		}
	);

	Logger::log_info("Successfully created Renderer at: ", get_address(this), "!");
}

Renderer::~Renderer() noexcept {
	Logger::log_info("Successfully destroyed a renderer!");
}

void Renderer::destroy() noexcept {
	this->~Renderer();
}

void Renderer::record_command_buffers() {
	// look at how D Y N A M I C this is
	begin_render_pass();

	bind_pipeline();

	_draw_queue.flush();

	end_render_pass();
}

void Renderer::draw() noexcept {
	Application::context()->add_to_render_queue([&]() { record_command_buffers(); }); // this is what I call "spicy" architecture
}

void Renderer::bind_descriptor(const VulkanDescriptor* descriptor) const noexcept {
	vkCmdBindDescriptorSets(Application::context()->commandBuffers().at(Application::context()->currentFrame()).get(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.layout(), 0, 1, descriptor->get_ptr(), 0, nullptr);
}

void Renderer::begin_render_pass() const noexcept {
	std::array<VkClearValue, 2> clear{};
	clear[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
	clear[1].depthStencil = { 1.0f, 0 };

	vkCmdBeginRenderPass(Application::context()->commandBuffers().at(Application::context()->currentFrame()).get(), &_framebuffers.get_begin_info(Application::context()->imageIndex(), clear), VK_SUBPASS_CONTENTS_INLINE);
}

void Renderer::end_render_pass() const noexcept {
	vkCmdEndRenderPass(Application::context()->commandBuffers().at(Application::context()->currentFrame()).get());
}

void Renderer::bind_pipeline() const noexcept {
	vkCmdBindPipeline(Application::context()->commandBuffers().at(Application::context()->currentFrame()).get(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.pipeline());
}

void Renderer::bind_model(const VulkanGPUBuffer* vertexBuffer, const VulkanGPUBuffer* indexBuffer) const noexcept {
	VkBuffer buffers[] = { vertexBuffer->buffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(Application::context()->commandBuffers().at(Application::context()->currentFrame()).get(), 0, 1, buffers, offsets);
	vkCmdBindIndexBuffer(Application::context()->commandBuffers().at(Application::context()->currentFrame()).get(), indexBuffer->buffer(), 0, VK_INDEX_TYPE_UINT32);
}

void Renderer::draw_model(const uint32 size) const noexcept {
	vkCmdDrawIndexed(Application::context()->commandBuffers().at(Application::context()->currentFrame()).get(), size, 1, 0, 0, 0);
}

} // namespace lyra
