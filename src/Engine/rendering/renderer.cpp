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

void Renderer::recreate_swapchain() {
    VulkanSwapchain oldSwapchain = std::move(var.swapchain);

    destroy_swapchain();

    var.swapchain.create(oldSwapchain);
    var.framebuffers.create();

    oldSwapchain.destroy();
}

void Renderer::update() {
    while (var.drawing) {
        draw();
    }
    
    var.device.wait();
}

void Renderer::record_command_buffers(int index) {
    // look at how D Y N A M I C this is
    var.commandBuffers[var.currentFrame].begin(0);

    begin_render_pass(var.currentFrame, var.framebuffers.begin_info(index));

    var.scene_queue.flush();

    var.bind_queue.flush();

    var.draw_queue.flush();

    end_render_pass(var.currentFrame);

    var.commandBuffers[var.currentFrame].end();
}

void Renderer::draw() {
    var.syncObjects.wait(var.currentFrame);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(var.device.get().device, var.swapchain.get().swapchain, UINT64_MAX, var.syncObjects.get().imageAvailableSemaphores[var.currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swapchain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        LOG_EXEPTION("Failed to get the next Vulkan image layer to blit on!");
    }

    var.syncObjects.reset(var.currentFrame);
    var.commandBuffers[var.currentFrame].reset();

    record_command_buffers(imageIndex);

    submit_device_queue(var.device.get().graphicsQueue, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

    present_queue(imageIndex);

    update_frame_count();
}

void Renderer::submit_device_queue(
    const VulkanDevice::VulkanQueueFamily   queue,
    const VulkanCommandBuffer               commandBuffer,
    const VulkanSyncObjects                 syncObjects,
    const uint32                            syncObjectIndex,
    const VkPipelineStageFlags              stageFlags
) const {
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
    if (vkQueueSubmit(queue.queue, 1, &submitInfo, syncObjects.get().inFlightFences[syncObjectIndex]) != VK_SUCCESS) LOG_EXEPTION("Failed to submit Vulkan queue!");
}

void Renderer::submit_device_queue(const VulkanDevice::VulkanQueueFamily queue, const VkPipelineStageFlags stageFlags) const {
    submit_device_queue(queue, var.commandBuffers[var.currentFrame], var.syncObjects, var.currentFrame, stageFlags);
}

void Renderer::present_queue(const uint32 imageIndex) {
    VkSwapchainKHR swapchains[] = { var.swapchain.get().swapchain };

    VkPresentInfoKHR presentInfo = {
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        nullptr,
        1,
        &var.syncObjects.get().renderFinishedSemaphores[var.currentFrame],
        1,
        swapchains,
        &imageIndex
    };

    VkResult result = vkQueuePresentKHR(var.device.get().presentQueue.queue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        recreate_swapchain();
        LOG_EXEPTION("Failed to get the next Vulkan image layer to blit on!");
    }
    else if (result != VK_SUCCESS) {
        LOG_EXEPTION("Failed to present swapchain image!");
    }
}

void Renderer::wait_device_queue(const VulkanDevice::VulkanQueueFamily queue) const {
    vkQueueWaitIdle(queue.queue);
}

/**
void Renderer::bind_descriptor(const VulkanDescriptor descriptor, VulkanGraphicsPipeline pipeline, int cmdBuffIndex) const {
    vkCmdBindDescriptorSets(var.commandBuffers[cmdBuffIndex].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get().pipelineLayout, 0, 1, descriptor.get().data(), 0, nullptr);
}
 */

void Renderer::begin_render_pass(int cmdBuffIndex, const VkRenderPassBeginInfo beginInfo) const {
	vkCmdBeginRenderPass(var.commandBuffers[cmdBuffIndex].get(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Renderer::end_render_pass(int cmdBuffIndex) const {
    vkCmdEndRenderPass(var.commandBuffers[cmdBuffIndex].get());
}

/** 
void Renderer::bind_pipeline(VulkanGraphicsPipeline pipeline, int cmdBuffIndex) const {
    vkCmdBindPipeline(var.commandBuffers[cmdBuffIndex].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get().graphicsPipeline);
}
 */

void Renderer::bind_model(const VkBuffer vertexBuffer, const VkBuffer indexBuffer, int cmdBuffIndex) const {
    vkCmdBindVertexBuffers(var.commandBuffers[cmdBuffIndex].get(), 0, 1, &vertexBuffer, 0);
    vkCmdBindIndexBuffer(var.commandBuffers[cmdBuffIndex].get(), indexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void Renderer::draw_model(const uint32 size, int cmdBuffIndex) const {
    vkCmdDrawIndexed(var.commandBuffers[cmdBuffIndex].get(), size, 1, 0, 0, 0);
}

void Renderer::update_frame_count() {
    var.currentFrame = (var.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

} // namespace lyra
