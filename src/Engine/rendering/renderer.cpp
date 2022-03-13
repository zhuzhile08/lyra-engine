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
    /**
    framebuffers.create_render_pass(device, swapchain);

    VulkanDescriptorSetLayout::Builder builder;
    
    builder.add_binding();

    descriptorSetLayout.create(device, builder);
    **/
}

void Renderer::destroySwapchain() {
    /// @todo image samplers and depth buffers

    framebuffers.destroy();
    swapchain.destroy();
}

void Renderer::recreateSwapchain() {

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
