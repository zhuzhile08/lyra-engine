// had to do this somewhere to implement the VMA functions, so I did it here
#include <rendering/renderer.h>

namespace lyra {

Renderer::Renderer() { }

void Renderer::destroy() {
    destroySwapchain();

    descriptorPool.destroy();
    descriptorSetLayout.destroy();
    syncObjects.destroy();
    commandPool.destroy();
    device.destroy();
    instance.destroy();
}

void Renderer::create(Window window) { 
    this->window = &window;

    instance.create(window);
    device.create(instance);
    commandPool.create(device);
    swapchain.create(device, instance, window);
    framebuffers.create_render_pass(device, swapchain);

    VulkanDescriptorSetLayout::Builder  layoutBuilder;
    layoutBuilder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1);
    descriptorSetLayout.create(device, layoutBuilder);

    framebuffers.create();
    
    VulkanDescriptorPool::Builder       poolBuilder;
    poolBuilder.set_max_sets(50);   // just some random number
    poolBuilder.add_pool_sizes(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT);
    descriptorPool.create(device, poolBuilder);

    for (auto& cmdBuff : commandBuffers) cmdBuff.create(device, commandPool);
    syncObjects.create(device, swapchain);
}

void Renderer::destroySwapchain() {
    framebuffers.destroy();
    swapchain.destroy();
}

void Renderer::recreateSwapchain() {
    VulkanSwapchain oldSwapchain = std::move(swapchain);

    destroySwapchain();

    swapchain.create(oldSwapchain);
    framebuffers.create();
}

void Renderer::submit_queue(
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

void Renderer::submit_queue(VulkanDevice::VulkanQueueFamily queue, uint32 index, VkPipelineStageFlags stageFlags) {
    submit_queue(queue, commandBuffers[index], syncObjects, index, stageFlags);
}

void Renderer::wait_queue(VulkanDevice::VulkanQueueFamily queue) {
    vkQueueWaitIdle(queue.queue);
}

void Renderer::bind_descriptor(const VulkanDescriptor descriptor, const VulkanGraphicsPipeline pipeline, const int cmdBuffIndex) const {
    vkCmdBindDescriptorSets(commandBuffers[cmdBuffIndex].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get().pipelineLayout, 0, 1, descriptor.get().data(), 0, nullptr);
}

void Renderer::begin_render_pass(const VulkanFramebuffers framebuffer, const int index, const VkClearValue clear, const int cmdBuffIndex) const {
	vkCmdBeginRenderPass(commandBuffers[cmdBuffIndex].get(), &framebuffer.begin_info(index, clear), VK_SUBPASS_CONTENTS_INLINE);
}

void Renderer::end_render_pass(const int cmdBuffIndex) const {
    vkCmdEndRenderPass(commandBuffers[cmdBuffIndex].get());
}

void Renderer::bind_pipeline(const VulkanGraphicsPipeline pipeline, const int cmdBuffIndex) const {
    vkCmdBindPipeline(commandBuffers[cmdBuffIndex].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get().graphicsPipeline);
}

} // namespace lyra
