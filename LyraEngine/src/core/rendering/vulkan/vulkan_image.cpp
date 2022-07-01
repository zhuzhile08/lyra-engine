#include <core/rendering/vulkan/vulkan_image.h>

namespace lyra {

VulkanImage::VulkanImage() { }

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
	else Logger::log_exception("Invalid image layout transition was requested whilst transitioning an image layout at: ", get_address(this));

	vkCmdPipelineBarrier(cmdBuff.get(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &get_image_memory_barrier(sourceAccess, destinationAccess, oldLayout, newLayout, subresourceRange));

	// end recording
	cmdBuff.end();

	// submit queues after recording
	cmdBuff.submit_queue(device->graphicsQueue().queue);
	cmdBuff.wait_queue(device->graphicsQueue().queue);
}

const VkFormat VulkanImage::get_best_format(const std::vector<VkFormat> candidates, const VkFormatFeatureFlags features, const VkImageTiling tiling) const {
	// check which tiling mode to use
	VkImageTiling tiling_ = VK_IMAGE_TILING_MAX_ENUM; // screw naming conventions, I don't care
	if (_tiling == VK_IMAGE_TILING_MAX_ENUM) tiling_ = tiling;
	else if (tiling == VK_IMAGE_TILING_MAX_ENUM) tiling_ = _tiling;
	else if (tiling != VK_IMAGE_TILING_MAX_ENUM && tiling_ != VK_IMAGE_TILING_MAX_ENUM) {
		Logger::log_warning("Defined 2 seperate tiling modes whilst finding the best format for a image: ", _tiling, " and ", tiling, "! Automatically set to the first mode!");
		tiling_ = _tiling;
	}
	else Logger::log_exception("No tiling mode was defined whilst attempting to find the best format for image: ", get_address(this), "!");

	for (const auto& format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(device->physicalDevice(), format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) return format;
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) return format;
	}

	Logger::log_exception("Failed to find supported format out of user-defined formats for image at: ", get_address(this), "!");

	return VK_FORMAT_MAX_ENUM;
}

} // namespace lyra