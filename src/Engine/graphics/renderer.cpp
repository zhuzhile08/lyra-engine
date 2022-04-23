#include <graphics/renderer.h>

namespace lyra {

Renderer::Renderer() { }

Renderer::~Renderer() noexcept {
	LOG_INFO("Successfully destroyed a renderer!");
}

void Renderer::destroy() noexcept {
	this->~Renderer();
}

void Renderer::create(Context* const context) {
	LOG_INFO("Creating Renderer...");

	this->context = context;

	_framebuffers.create(&context->device(), &context->swapchain());
	_pipeline.create(
		&context->device(),
		&_framebuffers, 
		&context->descriptorSetLayout(),
		{ { "data/shader/vert.spv", "main", VK_SHADER_STAGE_VERTEX_BIT }, { "data/shader/frag.spv", "main", VK_SHADER_STAGE_FRAGMENT_BIT } }, 
		context->swapchain().extent(),
		context->swapchain().extent()
	);

	LOG_INFO("Successfully created Renderer at: ", GET_ADDRESS(this), "!");
}

void Renderer::record_command_buffers() {
	// look at how D Y N A M I C this is
	begin_render_pass();

	bind_pipeline();

	_draw_queue.flush();

	end_render_pass();
}

void Renderer::draw() noexcept {
	context->add_to_render_queue([&]() { record_command_buffers(); });
}

void Renderer::bind_descriptor(const VulkanDescriptor* descriptor) const noexcept {
	vkCmdBindDescriptorSets(context->commandBuffers().at(context->currentFrame()).get(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.pipelineLayout(), 0, 1, descriptor->get_ptr(), 0, nullptr);
}

void Renderer::begin_render_pass() const noexcept {
	std::array<VkClearValue, 2> clear{};
	clear[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
	clear[1].depthStencil = { 1.0f, 0 };

	vkCmdBeginRenderPass(context->commandBuffers().at(context->currentFrame()).get(), &_framebuffers.get_begin_info(context->imageIndex(), clear), VK_SUBPASS_CONTENTS_INLINE);
}

void Renderer::end_render_pass() const noexcept {
	vkCmdEndRenderPass(context->commandBuffers().at(context->currentFrame()).get());
}

void Renderer::bind_pipeline() const noexcept {
	vkCmdBindPipeline(context->commandBuffers().at(context->currentFrame()).get(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.graphicsPipeline());
}

void Renderer::bind_model(const VulkanGPUBuffer* vertexBuffer, const VulkanGPUBuffer* indexBuffer) const noexcept {
	VkBuffer buffers[] = { vertexBuffer->buffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(context->commandBuffers().at(context->currentFrame()).get(), 0, 1, buffers, offsets);
	vkCmdBindIndexBuffer(context->commandBuffers().at(context->currentFrame()).get(), indexBuffer->buffer(), 0, VK_INDEX_TYPE_UINT32);
}

void Renderer::draw_model(const uint32 size) const noexcept {
	vkCmdDrawIndexed(context->commandBuffers().at(context->currentFrame()).get(), size, 1, 0, 0, 0);
}

} // namespace lyra
