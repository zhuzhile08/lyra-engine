#include <rendering/vulkan/vulkan_image.h>

namespace lyra {

VulkanImage::VulkanImage() { }

void VulkanImage::destroy_view() noexcept {
	vkDestroyImageView(device->device(), _view, nullptr);
	vkDestroyImage(device->device(), _image, nullptr);

	delete device;

	LOG_INFO("Succesfully destroyed Vulkan images!")
}

VkImageCreateInfo VulkanImage::get_image_create_info(
	VkFormat                format,
	VkExtent3D              extent,
	VkImageUsageFlags       usage,
	VkImageType             imageType,
	uint32_t                mipLevels,
	uint32_t                arrayLayers,
	VkSampleCountFlagBits   samples,
	VkImageTiling           tiling
) noexcept {
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
	this->device = new VulkanDevice(device);

	// image view creation info
	VkImageViewCreateInfo createInfo{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0,
		_image,
		viewType,
		format,
		colorComponents,
		subresourceRange
	};

	// create the view
	if (vkCreateImageView(device.device(), &createInfo, nullptr, &_view) != VK_SUCCESS) LOG_EXEPTION("Failed to create Vulkan image views")

	LOG_DEBUG(TAB, "Succesfully created Vulkan image view at ", GET_ADDRESS(this), "!")
}

} // namespace lyra
