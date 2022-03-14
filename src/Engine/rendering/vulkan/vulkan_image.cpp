#include <rendering/vulkan/vulkan_image.h>

namespace lyra {

VulkanImage::VulkanImage() { }

void VulkanImage::destroy() {
    vkDestroyImageView(device->get().device, view, nullptr);
    vkDestroyImage(device->get().device, image, nullptr);

    LOG_INFO("Succesfully destroyed Vulkan images!")
}

void VulkanImage::create(
    VulkanDevice            device, 
    VkFormat                format, 
    VkExtent3D              extent,
    VkImageUsageFlags       usage,
    VkImageSubresourceRange subresourceRange,
    VkImageType             imageType        = VK_IMAGE_TYPE_2D,
    uint32_t                mipLevels        = 1,
    uint32_t                arrayLayers      = 1,
    VkSampleCountFlagBits   samples          = VK_SAMPLE_COUNT_1_BIT,
    VkImageTiling           tiling           = VK_IMAGE_TILING_OPTIMAL,
    VkImageViewType         viewType         = VK_IMAGE_VIEW_TYPE_2D, 
    VkComponentMapping      colorComponents  = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY}
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

    // create the image
    vkCreateImage(device.get().device, &createInfo, nullptr, &image);

    // create the image view
    create_view(device, format, subresourceRange, viewType, colorComponents);

    LOG_INFO("Succesfully created Vulkan image and image view at ", GET_ADDRESS(this), "!", END_L)
}

void VulkanImage::create_view(
    VulkanDevice            device, 
    VkFormat                format, 
    VkImageSubresourceRange subresourceRange,
    VkImageViewType         viewType = VK_IMAGE_VIEW_TYPE_2D, 
    VkComponentMapping      colorComponents = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY}
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

	LOG_INFO("Succesfully created Vulkan image view at ", GET_ADDRESS(this), "!", END_L)
}

} // namespace lyra
