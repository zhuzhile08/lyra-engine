#include <rendering/render_stage.h>

namespace lyra {

RenderStage::RenderStage() { ("RenderStage", nullptr); }

void RenderStage::destroy() {
    var.framebuffers.destroy();
    var.pipeline.destroy();
    for (auto& commandBuffer : var.commandBuffers) commandBuffer.destroy();

    delete renderer;
}

void RenderStage::create(Renderer renderer) {
    set_parent(&renderer);
    this->renderer = new Renderer(renderer);

    var.framebuffers.create_render_pass(renderer.var.device, renderer.var.swapchain);
    var.framebuffers.create();
    for (auto& cmdBuff : var.commandBuffers) cmdBuff.create(renderer.var.device, renderer.var.commandPool);
}

void RenderStage::record_command_buffers() {
    // look at how D Y N A M I C this is
    var.commandBuffers[renderer->var.currentFrame].begin(0);

    begin_render_pass(var.framebuffers.begin_info(renderer->var.imageIndex));

    bind_pipeline();

    var.bind_queue.flush();

    var.draw_queue.flush();

    end_render_pass();

    var.commandBuffers[renderer->var.currentFrame].end();
}

void RenderStage::draw() {
    renderer->var.renderQueue.add([=]() { var.commandBuffers[renderer->var.currentFrame].reset(); record_command_buffers(); });
    renderer->var.submitQueue.add([=]() { renderer->submit_device_queue(renderer->var.device.get().presentQueue, var.commandBuffers[renderer->var.currentFrame], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT); });
}

void RenderStage::bind_descriptor(const VulkanDescriptor descriptor) const {
    vkCmdBindDescriptorSets(var.commandBuffers[renderer->get().currentFrame].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, var.pipeline.get().pipelineLayout, 0, 1, descriptor.get_ptr(), 0, nullptr);
}

void RenderStage::begin_render_pass(const VkRenderPassBeginInfo beginInfo) const {
    vkCmdBeginRenderPass(var.commandBuffers[renderer->get().currentFrame].get(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderStage::end_render_pass() const {
    vkCmdEndRenderPass(var.commandBuffers[renderer->get().currentFrame].get());
}

void RenderStage::bind_pipeline() const {
    vkCmdBindPipeline(var.commandBuffers[renderer->get().currentFrame].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, var.pipeline.get().graphicsPipeline);
}

void RenderStage::bind_model(const VkBuffer vertexBuffer, const VkBuffer indexBuffer) const {
    vkCmdBindVertexBuffers(var.commandBuffers[renderer->get().currentFrame].get(), 0, 1, &vertexBuffer, 0);
    vkCmdBindIndexBuffer(var.commandBuffers[renderer->get().currentFrame].get(), indexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void RenderStage::draw_model(const uint32 size) const {
    vkCmdDrawIndexed(var.commandBuffers[renderer->get().currentFrame].get(), size, 1, 0, 0, 0);
}

} // namespace lyra
