#include <rendering/vulkan/vulkan_image.h>

namespace lyra {

VulkanImage::VulkanImage() { }

void VulkanImage::destroy() {
    vkDestroyImageView(device->get().device, view, nullptr);

    LOG_INFO("Succesfully destroyed Vulkan images!")
}

VkImageCreateInfo VulkanImage::get_image_create_info(
    VulkanDevice            device, 
    VkFormat                format, 
    VkExtent3D              extent,
    VkImageUsageFlags       usage,
    VkImageType             imageType,
    uint32_t                mipLevels,
    uint32_t                arrayLayers,
    VkSampleCountFlagBits   samples,
    VkImageTiling           tiling
) {
    // image create info
    VkImageCreateInfo createInfo = {   
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        nullptr,
        0,
        imageType,
        format,
        extent,
        mipLevels,
        arrayLayers,
        samples,
        tiling,
        usage,
        VK_SHARING_MODE_EXCLUSIVE, /** @todo may come back to this area later */
        0,
        0,
        VK_IMAGE_LAYOUT_UNDEFINED
    };

    return createInfo;
}

void VulkanImage::create_view(
    VulkanDevice            device, 
    VkFormat                format, 
    VkImageSubresourceRange subresourceRange,
    VkImageViewType         viewType, 
    VkComponentMapping      colorComponents
) {
    this->device = &device;

    // image view creation info
    VkImageViewCreateInfo createInfo {
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        nullptr,
        0,
        image,
        viewType,
        format,
        colorComponents,
        subresourceRange
    };

    // create the view
    if(vkCreateImageView(device.get().device, &createInfo, nullptr, &view) != VK_SUCCESS) LOG_EXEPTION("Failed to create Vulkan image views")

	LOG_DEBUG(TAB, "Succesfully created Vulkan image view at ", GET_ADDRESS(this), "!")
}

} // namespace lyra
