#include <rendering/vulkan/vulkan_image.h>

namespace lyra {

VulkanImage::VulkanImage() { }

void VulkanImage::destroy_view() noexcept {
	vkDestroyImageView(device->device(), _view, nullptr);
	vkDestroyImage(device->device(), _image, nullptr);

	LOG_INFO("Succesfully destroyed Vulkan images!")
}

VkImageCreateInfo VulkanImage::get_image_create_info(
	VkFormat format,
	VkExtent3D extent,
	VkImageUsageFlags usage,
	VkImageType imageType,
	uint32 mipLevels,
	uint32 arrayLayers,
	VkSampleCountFlagBits samples,
	VkImageTiling tiling
) noexcept {
	_tiling = tiling;

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
	const VulkanDevice* device,
	VkFormat format,
	VkImageSubresourceRange subresourceRange,
	VkImageViewType viewType,
	VkComponentMapping colorComponents
) {
	this->device = device;

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
	if (vkCreateImageView(device->device(), &createInfo, nullptr, &_view) != VK_SUCCESS) LOG_EXEPTION("Failed to create Vulkan image views")

	LOG_DEBUG(TAB, "Succesfully created Vulkan image view at ", GET_ADDRESS(this), "!")
}

void VulkanImage::transition_layout(VulkanDevice device, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkFormat format, const VkImageAspectFlagBits aspect, const VulkanCommandPool commandPool) const {
	// temporary command buffer for copying
	VulkanCommandBuffer     cmdBuff;
	cmdBuff.create(&device, &commandPool);
	// begin recording
	cmdBuff.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkImageMemoryBarrier barrier = {
		VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		nullptr,
		0,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		oldLayout,
		newLayout,
		VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED,
		_image,
		{aspect, 0, 1, 0, 1}
	};

	VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else LOG_EXEPTION("Invalid image layout transition was requested whilst transitioning an image layout at: ", GET_ADDRESS(this))

	vkCmdPipelineBarrier(cmdBuff.get(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	// end recording
	cmdBuff.end();

	// submit queues after recording
	cmdBuff.submit_queue(device.graphicsQueue().queue);
	cmdBuff.wait_queue(device.graphicsQueue().queue);

	// destroy command buffer
	cmdBuff.destroy();
}

const VkFormat VulkanImage::get_best_format(VulkanDevice device, const std::vector<VkFormat> candidates, VkFormatFeatureFlags features, VkImageTiling tiling) const {
	// check which tiling mode to use
	VkImageTiling tiling_; // screw naming conventions, I don't care
	if (_tiling == VK_IMAGE_TILING_MAX_ENUM) tiling_ = tiling;
	else if (tiling == VK_IMAGE_TILING_MAX_ENUM) tiling_ = _tiling;
	else if (tiling != VK_IMAGE_TILING_MAX_ENUM && tiling != VK_IMAGE_TILING_MAX_ENUM) {
		LOG_WARNING("Defined 2 seperate tiling modes whilst finding the best format for a image: ", _tiling, " and ", tiling, "! Automatically set to the first mode!")
		tiling_ = _tiling;
	} else LOG_EXEPTION("No tiling mode was defined whilst attempting to find the best format for image: ", GET_ADDRESS(this), "!")

	for (const auto& format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(device.physicalDevice(), format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) return format;
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) return format;
	}

	LOG_EXEPTION("Failed to find supported format out of user-defined formats for image at: ", GET_ADDRESS(this), "!")
}

} // namespace lyra
