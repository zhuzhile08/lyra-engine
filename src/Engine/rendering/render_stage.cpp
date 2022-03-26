#include <rendering/render_stage.h>

namespace lyra {

RenderStage::RenderStage() { ("RenderStage", nullptr); }

void RenderStage::destroy() noexcept {
    _framebuffers.destroy();
    _pipeline.destroy();
    for (auto& commandBuffer : _commandBuffers) commandBuffer.destroy();

    delete renderer;
}

void RenderStage::create(Renderer renderer) {
    set_parent(&renderer);
    this->renderer = new Renderer(renderer);

    _framebuffers.create(renderer._device, renderer._swapchain);
    for (auto& cmdBuff : _commandBuffers) cmdBuff.create(renderer._device, renderer._commandPool);
}

void RenderStage::record_command_buffers() {
    // look at how D Y N A M I C this is
    _commandBuffers[renderer->currentFrame()].begin(0);

    begin_render_pass(_framebuffers.begin_info(renderer->_imageIndex));

    bind_pipeline();

    _bind_queue.flush();

    _draw_queue.flush();

    end_render_pass();

    _commandBuffers[renderer->currentFrame()].end();
}

void RenderStage::draw() noexcept {
    renderer->_renderQueue.add([=]() { _commandBuffers[renderer->currentFrame()].reset(); record_command_buffers(); });
    renderer->_submitQueue.add([=]() { renderer->submit_device_queue(renderer->_device.presentQueue(), _commandBuffers[renderer->currentFrame()], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT); });
}

void RenderStage::bind_descriptor(const VulkanDescriptor descriptor) const noexcept {
    vkCmdBindDescriptorSets(_commandBuffers[renderer->currentFrame()].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.pipelineLayout(), 0, 1, descriptor.get_ptr(), 0, nullptr);
}

void RenderStage::begin_render_pass(const VkRenderPassBeginInfo beginInfo) const noexcept {
    vkCmdBeginRenderPass(_commandBuffers[renderer->currentFrame()].get(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderStage::end_render_pass() const noexcept {
    vkCmdEndRenderPass(_commandBuffers[renderer->currentFrame()].get());
}

void RenderStage::bind_pipeline() const noexcept {
    vkCmdBindPipeline(_commandBuffers[renderer->currentFrame()].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.graphicsPipeline());
}

void RenderStage::bind_model(const VkBuffer vertexBuffer, const VkBuffer indexBuffer) const noexcept {
    vkCmdBindVertexBuffers(_commandBuffers[renderer->currentFrame()].get(), 0, 1, &vertexBuffer, 0);
    vkCmdBindIndexBuffer(_commandBuffers[renderer->currentFrame()].get(), indexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void RenderStage::draw_model(const uint32 size) const noexcept {
    vkCmdDrawIndexed(_commandBuffers[renderer->currentFrame()].get(), size, 1, 0, 0, 0);
}

} // namespace lyra
