#include <rendering/render_stage.h>

namespace lyra {

RenderStage::RenderStage() { ("RenderStage", nullptr); }

void RenderStage::destroy() {
    var.framebuffers.destroy();
    var.pipeline.destroy();
    var.descriptors.destroy();
    for (auto& commandBuffer : var.commandBuffers) commandBuffer.destroy();
}

void RenderStage::create(Renderer renderer, uint16 descriptorCount, VulkanDescriptor::Writer writer) {
    set_parent(&renderer);

    var.framebuffers.create_render_pass(renderer.var.device, renderer.var.swapchain);
    var.framebuffers.create();
    for (auto& cmdBuff : var.commandBuffers) cmdBuff.create(renderer.var.device, renderer.var.commandPool);
    var.descriptors.create(renderer.var.device, descriptorCount, renderer.var.descriptorSetLayout, renderer.var.descriptorPool, writer);
}

void RenderStage::record_command_buffers(int index, int currentFrame) {
    // look at how D Y N A M I C this is
    var.commandBuffers[currentFrame].begin(0);

    begin_render_pass(currentFrame, var.framebuffers.begin_info(index));

    bind_pipeline(var.pipeline, currentFrame);

    var.bind_queue.flush();

    bind_descriptor(var.descriptors, var.pipeline, currentFrame);

    var.draw_queue.flush();

    end_render_pass(currentFrame);

    var.commandBuffers[currentFrame].end();
}

void RenderStage::draw(Renderer renderer) {
    renderer.var.renderQueue.add([&]() { var.commandBuffers[renderer.var.currentFrame].reset(); });
    renderer.var.renderQueue.add([&]() { record_command_buffers(renderer.get().imageIndex, renderer.var.currentFrame); });
    renderer.var.submitQueue.add([&]() { renderer.submit_device_queue(renderer.var.device.get().presentQueue, var.commandBuffers[renderer.var.currentFrame], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT); });
}

void RenderStage::bind_descriptor(const VulkanDescriptor descriptor, const VulkanGraphicsPipeline pipeline, int cmdBuffIndex) const {
    vkCmdBindDescriptorSets(var.commandBuffers[cmdBuffIndex].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get().pipelineLayout, 0, 1, descriptor.get().data(), 0, nullptr);
}

void RenderStage::begin_render_pass(int cmdBuffIndex, const VkRenderPassBeginInfo beginInfo) const {
    vkCmdBeginRenderPass(var.commandBuffers[cmdBuffIndex].get(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderStage::end_render_pass(int cmdBuffIndex) const {
    vkCmdEndRenderPass(var.commandBuffers[cmdBuffIndex].get());
}

void RenderStage::bind_pipeline(VulkanGraphicsPipeline pipeline, int cmdBuffIndex) const {
    vkCmdBindPipeline(var.commandBuffers[cmdBuffIndex].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get().graphicsPipeline);
}

void RenderStage::bind_model(const VkBuffer vertexBuffer, const VkBuffer indexBuffer, int cmdBuffIndex) const {
    vkCmdBindVertexBuffers(var.commandBuffers[cmdBuffIndex].get(), 0, 1, &vertexBuffer, 0);
    vkCmdBindIndexBuffer(var.commandBuffers[cmdBuffIndex].get(), indexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void RenderStage::draw_model(const uint32 size, int cmdBuffIndex) const {
    vkCmdDrawIndexed(var.commandBuffers[cmdBuffIndex].get(), size, 1, 0, 0, 0);
}

} // namespace lyra
