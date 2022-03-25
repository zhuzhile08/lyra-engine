#include <rendering/renderer.h>

namespace lyra {

Renderer::Renderer() { ("Renderer", nullptr); }

void Renderer::destroy() noexcept {
    destroy_swapchain();

    var.descriptorPool.destroy();
    var.descriptorSetLayout.destroy();
    var.syncObjects.destroy();
    var.commandPool.destroy();
    var.device.destroy();
    var.instance.destroy();

    delete window;
}

void Renderer::create(Window window) { 
    this->window = new Window(window);

    var.instance.create(window);
    var.device.create(var.instance);
    var.commandPool.create(var.device);
    var.swapchain.create(var.device, var.instance, window);

    VulkanDescriptorSetLayout::Builder  layoutBuilder;
    layoutBuilder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1);
    layoutBuilder.add_binding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_VERTEX_BIT, 1);
    var.descriptorSetLayout.create(var.device, layoutBuilder);
    
    VulkanDescriptorPool::Builder       poolBuilder;
    poolBuilder.set_max_sets(50);   // just some random number
    poolBuilder.add_pool_sizes(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT);
    poolBuilder.add_pool_sizes(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT);
    var.descriptorPool.create(var.device, poolBuilder);

    var.syncObjects.create(var.device, var.swapchain);
}

void Renderer::destroy_swapchain() noexcept {
    var.swapchain.destroy();
}

void Renderer::recreate_swapchain() {
    VulkanSwapchain oldSwapchain = std::move(var.swapchain);

    destroy_swapchain();

    var.swapchain.create(oldSwapchain);

    oldSwapchain.destroy();
}

void Renderer::update() {
    while (var.running) {
        draw();
    }
    
    var.device.wait();
}

void Renderer::draw() {
    var.syncObjects.wait(var.currentFrame);

    VkResult result = vkAcquireNextImageKHR(var.device.get().device, var.swapchain.get().swapchain, UINT64_MAX, var.syncObjects.get().imageAvailableSemaphores[var.currentFrame], VK_NULL_HANDLE, &var.imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swapchain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) LOG_EXEPTION("Failed to get the next Vulkan image layer to blit on!");

    var.syncObjects.reset(var.currentFrame);

    var.renderQueue.flush();

    var.submitQueue.flush();

    present_queue(var.imageIndex);

    update_frame_count();
}

void Renderer::submit_device_queue(
    const VulkanDevice::VulkanQueueFamily   queue,
    const VulkanCommandBuffer               commandBuffer
) const {
    // queue submission info
    VkSubmitInfo submitInfo = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,
        nullptr,
        0,
        nullptr,
        nullptr,
        1,
        commandBuffer.get_ptr(),
        0,
        nullptr
    };

    // submit the queue
    if (vkQueueSubmit(queue.queue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) LOG_EXEPTION("Failed to submit Vulkan queue!");
}

void Renderer::submit_device_queue(const VulkanDevice::VulkanQueueFamily queue, const VulkanCommandBuffer commandBuffer, const VkPipelineStageFlags stageFlags) const {
    VkSemaphore waitSemaphores[] = { var.syncObjects.get().imageAvailableSemaphores[var.currentFrame] };
    VkSemaphore signalSemaphores[] = { var.syncObjects.get().renderFinishedSemaphores[var.currentFrame] };

    VkSubmitInfo submitInfo = {
       VK_STRUCTURE_TYPE_SUBMIT_INFO,
       nullptr,
       1,
       waitSemaphores,
       &stageFlags,
       1,
       commandBuffer.get_ptr(),
       1,
       signalSemaphores
    };

    // submit the queue
    if (vkQueueSubmit(queue.queue, 1, &submitInfo, var.syncObjects.get().inFlightFences[var.currentFrame]) != VK_SUCCESS) LOG_EXEPTION("Failed to submit Vulkan queue!");
}

void Renderer::present_queue(const uint32 imageIndex) {
    VkSwapchainKHR swapchains[] = { var.swapchain.get().swapchain };
    VkSemaphore signalSemaphores[] = { var.syncObjects.get().renderFinishedSemaphores[var.currentFrame] };

    VkPresentInfoKHR presentInfo = {
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        nullptr,
        1,
        signalSemaphores,
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

void Renderer::update_frame_count() noexcept {
    var.currentFrame = (var.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

Renderer::Variables Renderer::get() const noexcept {
    return var;
}

} // namespace lyra
