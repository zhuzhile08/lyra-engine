#include <rendering/renderer.h>

namespace lyra {

Renderer::Renderer() { }

void Renderer::destroy() {
    destroy_swapchain();

    var.descriptorPool.destroy();
    var.descriptorSetLayout.destroy();
    var.syncObjects.destroy();
    var.commandPool.destroy();
    var.device.destroy();
    var.instance.destroy();
}

void Renderer::create(Window window) { 
    this->window = &window;

    var.instance.create(window);
    var.device.create(var.instance);
    var.commandPool.create(var.device);
    var.swapchain.create(var.device, var.instance, window);
    var.framebuffers.create_render_pass(var.device, var.swapchain);

    VulkanDescriptorSetLayout::Builder  layoutBuilder;
    layoutBuilder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1);
    layoutBuilder.add_binding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_VERTEX_BIT, 1);
    var.descriptorSetLayout.create(var.device, layoutBuilder);

    var.framebuffers.create();
    
    VulkanDescriptorPool::Builder       poolBuilder;
    poolBuilder.set_max_sets(50);   // just some random number
    poolBuilder.add_pool_sizes(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT);
    poolBuilder.add_pool_sizes(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT);
    var.descriptorPool.create(var.device, poolBuilder);

    for (auto& cmdBuff : var.commandBuffers) cmdBuff.create(var.device, var.commandPool);
    var.syncObjects.create(var.device, var.swapchain);
}

void Renderer::destroy_swapchain() {
    var.framebuffers.destroy();
    var.swapchain.destroy();
}

void Renderer::draw() {

}

void Renderer::update() {
    
}

void Renderer::recreate_swapchain() {
    VulkanSwapchain oldSwapchain = std::move(var.swapchain);

    destroy_swapchain();

    var.swapchain.create(oldSwapchain);
    var.framebuffers.create();

    oldSwapchain.destroy();
}

void Renderer::submit_device_queue(
    VulkanDevice::VulkanQueueFamily queue, 
    VulkanCommandBuffer             commandBuffer,
    VulkanSyncObjects               syncObjects, 
    uint32                          syncObjectIndex,
    VkPipelineStageFlags            stageFlags
) {
    // queue submission info
    VkSubmitInfo submitInfo = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,
        nullptr,
        0,
        &syncObjects.get().imageAvailableSemaphores[syncObjectIndex],
        &stageFlags,
        1,
        commandBuffer.get_ptr(),
        1,
        &syncObjects.get().renderFinishedSemaphores[syncObjectIndex]
    };

    // submit the queue
    vkQueueSubmit(queue.queue, 1, &submitInfo, syncObjects.get().inFlightFences[syncObjectIndex]);
}

void Renderer::submit_device_queue(VulkanDevice::VulkanQueueFamily queue, uint32 index, VkPipelineStageFlags stageFlags) {
    submit_device_queue(queue, var.commandBuffers[index], var.syncObjects, index, stageFlags);
}

void Renderer::wait_device_queue(VulkanDevice::VulkanQueueFamily queue) {
    vkQueueWaitIdle(queue.queue);
}

void Renderer::bind_descriptor(const VulkanDescriptor descriptor, const VulkanGraphicsPipeline pipeline, const int cmdBuffIndex) const {
    vkCmdBindDescriptorSets(var.commandBuffers[cmdBuffIndex].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get().pipelineLayout, 0, 1, descriptor.get().data(), 0, nullptr);
}

void Renderer::begin_render_pass(const VulkanFramebuffers framebuffer, const int index, const VkClearValue clear, const int cmdBuffIndex) const {
	vkCmdBeginRenderPass(var.commandBuffers[cmdBuffIndex].get(), &framebuffer.begin_info(index, clear), VK_SUBPASS_CONTENTS_INLINE);
}

void Renderer::end_render_pass(const int cmdBuffIndex) const {
    vkCmdEndRenderPass(var.commandBuffers[cmdBuffIndex].get());
}

void Renderer::bind_pipeline(const VulkanGraphicsPipeline pipeline, const int cmdBuffIndex) const {
    vkCmdBindPipeline(var.commandBuffers[cmdBuffIndex].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get().graphicsPipeline);
}

} // namespace lyra
