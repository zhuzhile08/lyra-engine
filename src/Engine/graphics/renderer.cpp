#include <graphics/renderer.h>

namespace lyra {

Renderer::Renderer() { ("Renderer", nullptr); }

void Renderer::destroy() noexcept {
    _framebuffers.destroy();
    _pipeline.destroy();
    for (auto& commandBuffer : _commandBuffers) commandBuffer.destroy();
}

void Renderer::create(Context* context) {
    this->context = context;

    _framebuffers.create(&context->_device, &context->_swapchain);
    _commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    for (auto& cmdBuff : _commandBuffers) cmdBuff.create(&context->_device, &context->_commandPool);
    _pipeline.create(&context->_device, _framebuffers, context->descriptorSetLayout(), {{"data/shader/vert.spv", "main", VK_SHADER_STAGE_VERTEX_BIT}, {"data/shader/frag.spv", "main", VK_SHADER_STAGE_FRAGMENT_BIT}}, context->swapchain().extent(), context->swapchain().extent());
}

void Renderer::record_command_buffers() {
    // look at how D Y N A M I C this is
    _commandBuffers[context->currentFrame()].begin(0);

    begin_render_pass();

    bind_pipeline();

    _bind_queue.flush();

    _draw_queue.flush();

    end_render_pass();

    _commandBuffers[context->currentFrame()].end();
}

void Renderer::draw() noexcept {
    context->_renderQueue.add([&]() { _commandBuffers[context->currentFrame()].reset(); record_command_buffers(); });
    context->_submitQueue.add([&]() { context->submit_device_queue(context->_device.presentQueue(), _commandBuffers[context->currentFrame()], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT); });
}

void Renderer::bind_descriptor(const VulkanDescriptor descriptor) const noexcept {
    vkCmdBindDescriptorSets(_commandBuffers[context->currentFrame()].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.pipelineLayout(), 0, 1, descriptor.get_ptr(), 0, nullptr);
}

void Renderer::begin_render_pass() const noexcept {
    std::array<VkClearValue, 2> clear{};
    clear[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clear[1].depthStencil = { 1.0f, 0 };

    vkCmdBeginRenderPass(_commandBuffers[context->currentFrame()].get(), &_framebuffers.get_begin_info(context->imageIndex(), clear), VK_SUBPASS_CONTENTS_INLINE);
}

void Renderer::end_render_pass() const noexcept {
    vkCmdEndRenderPass(_commandBuffers[context->currentFrame()].get());
}

void Renderer::bind_pipeline() const noexcept {
    vkCmdBindPipeline(_commandBuffers[context->currentFrame()].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.graphicsPipeline());
}

void Renderer::bind_model(const VkBuffer vertexBuffer, const VkBuffer indexBuffer) const noexcept {
    vkCmdBindVertexBuffers(_commandBuffers[context->currentFrame()].get(), 0, 1, &vertexBuffer, 0);
    vkCmdBindIndexBuffer(_commandBuffers[context->currentFrame()].get(), indexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void Renderer::draw_model(const uint32 size) const noexcept {
    vkCmdDrawIndexed(_commandBuffers[context->currentFrame()].get(), size, 1, 0, 0, 0);
}

} // namespace lyra
