#include <core/rendering/vulkan/vulkan_image.h>

#include <core/logger.h>

#include <core/context.h>
#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/command_buffer.h>

namespace lyra {

VulkanImage::~VulkanImage() {
	vkDestroyImageView(Context::get()->renderSystem()->device()->device(), _view, nullptr);
	vkDestroyImage(Context::get()->renderSystem()->device()->device(), _image, nullptr);

	Logger::log_debug(Logger::tab(), "Successfully destroyed Vulkan images!");
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
	lassert(vkCreateImageView(Context::get()->renderSystem()->device()->device(), &createInfo, nullptr, &_view) == VK_SUCCESS, "Failed to create Vulkan image views");

	Logger::log_debug(Logger::tab(), "Successfully created Vulkan image view at ", get_address(this), "!");
}

void VulkanImage::transition_layout(
	const VkImageLayout oldLayout,
	const VkImageLayout newLayout,
	const VkFormat format,
	const VkImageSubresourceRange subresourceRange
) const {
	// get a command buffer for setting up memory barrier
	CommandBuffer cmdBuff = Context::get()->renderSystem()->commandBuffers()->get_unused();
	// begin recording
	Context::get()->renderSystem()->commandBuffers()->begin(cmdBuff, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

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

	vkCmdPipelineBarrier(Context::get()->renderSystem()->commandBuffers()->commandBuffer(cmdBuff)->commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &get_image_memory_barrier(sourceAccess, destinationAccess, oldLayout, newLayout, subresourceRange));

	// end recording
	Context::get()->renderSystem()->commandBuffers()->end(cmdBuff);

	// submit queues after recording
	Context::get()->renderSystem()->commandBuffers()->submit_queue(cmdBuff, Context::get()->renderSystem()->device()->graphicsQueue().queue);
	Context::get()->renderSystem()->commandBuffers()->wait_queue(Context::get()->renderSystem()->device()->graphicsQueue().queue);
	// reset the command buffer
	Context::get()->renderSystem()->commandBuffers()->reset(cmdBuff);
}

const VkFormat VulkanImage::get_best_format(const std::vector<VkFormat> candidates, const VkFormatFeatureFlags features, const VkImageTiling tiling) const {
	// check which tiling mode to use
	VkImageTiling tiling_; // screw naming conventions, I don't care
	if (_tiling == VK_IMAGE_TILING_MAX_ENUM) 
		tiling_ = tiling;
	else if (tiling == VK_IMAGE_TILING_MAX_ENUM) 
		tiling_ = _tiling;
	else if (_tiling == VK_IMAGE_TILING_MAX_ENUM && tiling == VK_IMAGE_TILING_MAX_ENUM) 
		Logger::log_exception("No tiling mode was defined whilst attempting to find the best format for image: ", get_address(this), "!");
	else if (_tiling != VK_IMAGE_TILING_MAX_ENUM && tiling != VK_IMAGE_TILING_MAX_ENUM) {
		tiling_ = tiling;
	}

	for (uint32 i = 0; i < candidates.size(); i++) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(Context::get()->renderSystem()->device()->physicalDevice(), candidates.at(i), &props);

		if (tiling_ == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) return candidates.at(i);
		else if (tiling_ == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) return candidates.at(i);

	}

	Logger::log_exception("Failed to find supported format out of user-defined formats for image at: ", get_address(this), "!");

	return VK_FORMAT_MAX_ENUM;
}

} // namespace lyra
