#include <core/rendering/vulkan/vulkan_image.h>

namespace lyra {

VulkanImage::VulkanImage() { }

VulkanImage::~VulkanImage() noexcept {
	vkDestroyImageView(device->device(), _view, nullptr);
	vkDestroyImage(device->device(), _image, nullptr);

	LOG_DEBUG(TAB, "Succesfully destroyed Vulkan images!"); // this message suddenly comes up during the execution of the create_render_pass function and I have no idea why... Nothing seems wrong tho?
}

void VulkanImage::destroy() noexcept {
	this->~VulkanImage();
}

const VkImageCreateInfo VulkanImage::get_image_create_info(
	const VkFormat format,
	const VkExtent3D extent,
	const VkImageUsageFlags usage,
	const uint32 mipLevels,
	const VkImageType imageType,
	const uint32 arrayLayers,
	const VkSampleCountFlagBits samples,
	const VkImageTiling tiling
) noexcept {
	_tiling = tiling;

	return {
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
}

void VulkanImage::create_view(const VkFormat format, const VkImageSubresourceRange subresourceRange, const VkImageViewType viewType, const VkComponentMapping colorComponents) {
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
	if (vkCreateImageView(device->device(), &createInfo, nullptr, &_view) != VK_SUCCESS) LOG_EXEPTION("Failed to create Vulkan image views");

	LOG_DEBUG(TAB, "Succesfully created Vulkan image view at ", get_address(this), "!");
}

void VulkanImage::create_view(const VulkanDevice* const device, const VkFormat format, const VkImageSubresourceRange subresourceRange, const VkImageViewType viewType, const VkComponentMapping colorComponents) {
	this->device = device;

	create_view(format, subresourceRange, viewType, colorComponents);
}

void VulkanImage::transition_layout(
	const VulkanCommandPool* commandPool,
	const VkImageLayout oldLayout,
	const VkImageLayout newLayout,
	const VkFormat format,
	const VkImageSubresourceRange subresourceRange
) const {
	// temporary command buffer for setting up memory barrier
	VulkanCommandBuffer     cmdBuff;
	cmdBuff.create(device, commandPool);
	// begin recording
	cmdBuff.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	VkAccessFlags sourceAccess = 0;
	VkAccessFlags destinationAccess = VK_ACCESS_TRANSFER_WRITE_BIT;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		sourceAccess = 0; destinationAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		sourceAccess = VK_ACCESS_TRANSFER_WRITE_BIT; destinationAccess = VK_ACCESS_SHADER_READ_BIT;
		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT; destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		sourceAccess = 0; destinationAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else LOG_EXEPTION("Invalid image layout transition was requested whilst transitioning an image layout at: ", get_address(this));

	vkCmdPipelineBarrier(cmdBuff.get(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &get_image_memory_barrier(sourceAccess, destinationAccess, oldLayout, newLayout, subresourceRange));

	// end recording
	cmdBuff.end();

	// submit queues after recording
	cmdBuff.submit_queue(device->graphicsQueue().queue);
	cmdBuff.wait_queue(device->graphicsQueue().queue);
}

void VulkanImage::transition_layout(
	const VulkanDevice* const device,
	const VulkanCommandPool* const commandPool,
	const VkImageLayout oldLayout, 
	const VkImageLayout newLayout, 
	const VkFormat format, 
	const VkImageSubresourceRange subresourceRange
) {
	this->device = device;

	transition_layout(commandPool, oldLayout, newLayout, format, subresourceRange);
}

const VkImageMemoryBarrier VulkanImage::get_image_memory_barrier(
	const VkAccessFlags srcAccessMask,
	const VkAccessFlags dstAccessMask,
	const VkImageLayout srcLayout,
	const VkImageLayout dstLayout,
	const VkImageSubresourceRange subresourceRange,
	const uint32_t srcQueueFamily,
	const uint32_t dstQueueFamily
) const noexcept {
	return {
		VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		nullptr,
		srcAccessMask,
		dstAccessMask,
		srcLayout,
		dstLayout,
		srcQueueFamily,
		dstQueueFamily,
		_image,
		subresourceRange
	};
}

const VkFormat VulkanImage::get_best_format(const std::vector<VkFormat> candidates, const VkFormatFeatureFlags features, const VkImageTiling tiling) const {
	// check which tiling mode to use
	VkImageTiling tiling_ = VK_IMAGE_TILING_MAX_ENUM; // screw naming conventions, I don't care
	if (_tiling == VK_IMAGE_TILING_MAX_ENUM) tiling_ = tiling;
	else if (tiling == VK_IMAGE_TILING_MAX_ENUM) tiling_ = _tiling;
	else if (tiling != VK_IMAGE_TILING_MAX_ENUM && tiling_ != VK_IMAGE_TILING_MAX_ENUM) {
		LOG_WARNING("Defined 2 seperate tiling modes whilst finding the best format for a image: ", _tiling, " and ", tiling, "! Automatically set to the first mode!");
		tiling_ = _tiling;
	}
	else LOG_EXEPTION("No tiling mode was defined whilst attempting to find the best format for image: ", get_address(this), "!");

	for (const auto& format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(device->physicalDevice(), format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) return format;
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) return format;
	}

	LOG_EXEPTION("Failed to find supported format out of user-defined formats for image at: ", get_address(this), "!");

	return VK_FORMAT_MAX_ENUM;
}

const VkFormat VulkanImage::get_best_format(const VulkanDevice* const device, const std::vector<VkFormat> candidates, const VkFormatFeatureFlags features, const VkImageTiling tiling) {
	this->device = device;

	return get_best_format(candidates, features, tiling);
}

} // namespace lyra
