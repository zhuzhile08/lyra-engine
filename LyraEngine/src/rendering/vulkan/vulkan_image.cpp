#include <rendering/vulkan/vulkan_image.h>

#include <core/logger.h>

#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/command_buffer.h>

#include <core/application.h>

namespace lyra {

namespace vulkan {

Image::~Image() {
	// destroy the image view
	vkDestroyImageView(Application::renderSystem()->device()->device(), m_view, nullptr);
	// destroy the image
	vkDestroyImage(Application::renderSystem()->device()->device(), m_image, nullptr);

	Logger::log_debug(Logger::tab(), "Successfully destroyed Vulkan images!");
}

const VkImageCreateInfo Image::get_image_create_info(
	const VkFormat format,
	const VkExtent3D extent,
	const VkImageUsageFlags usage,
	const uint32 mipLevels,
	const VkImageType imageType,
	const uint32 arrayLayers,
	const VkSampleCountFlagBits samples,
	const VkImageTiling tiling
) noexcept {
	m_tiling = tiling;

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

void Image::create_view(const VkFormat format, const VkImageSubresourceRange subresourceRange, const VkImageViewType viewType, const VkComponentMapping colorComponents) {
	// image view creation info
	VkImageViewCreateInfo createInfo{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0,
		m_image,
		viewType,
		format,
		colorComponents,
		subresourceRange
	};

	// create the view
	vassert(vkCreateImageView(Application::renderSystem()->device()->device(), &createInfo, nullptr, &m_view), "create Vulkan image views");

	Logger::log_debug(Logger::tab(), "Successfully created Vulkan image view at ", get_address(this), "!");
}

void Image::transition_layout(
	const VkImageLayout oldLayout,
	const VkImageLayout newLayout,
	const VkFormat format,
	const VkImageSubresourceRange subresourceRange
) const {
	// get a command buffer for setting up memory barrier
	CommandBuffer cmdBuff(Application::renderSystem()->commandBuffers());
	// begin recording
	cmdBuff.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// goofy ahh check to see which image layout to use
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

	// use a memory barrier to transition the layout
	cmdBuff.pipelineBarrier(
		sourceStage, 
		destinationStage, 
		VkMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM },
		VkBufferMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM }, 
		get_image_memory_barrier(
			sourceAccess, 
			destinationAccess, 
			oldLayout, 
			newLayout, 
			subresourceRange
		)
	);

	// end recording
	cmdBuff.end();
	// submit queues after recording
	cmdBuff.submitQueue(Application::renderSystem()->device()->graphicsQueue().queue);
	// reset the command buffer
	cmdBuff.reset();
}

const VkFormat Image::get_best_format(const std::vector<VkFormat> candidates, const VkFormatFeatureFlags features, const VkImageTiling tiling) const {
	// check which tiling mode to use
	VkImageTiling tiling_; // screw naming conventions, I don't care
	if (m_tiling == VK_IMAGE_TILING_MAX_ENUM) 
		tiling_ = tiling;
	else if (tiling == VK_IMAGE_TILING_MAX_ENUM) 
		tiling_ = m_tiling;
	else if (m_tiling == VK_IMAGE_TILING_MAX_ENUM && tiling == VK_IMAGE_TILING_MAX_ENUM) 
		Logger::log_exception("No tiling mode was defined whilst attempting to find the best format for image: ", get_address(this), "!");
	else if (m_tiling != VK_IMAGE_TILING_MAX_ENUM && tiling != VK_IMAGE_TILING_MAX_ENUM) {
		tiling_ = tiling;
	}

	// check which of the canditates is the best choice
	for (uint32 i = 0; i < candidates.size(); i++) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(Application::renderSystem()->device()->physicalDevice(), candidates.at(i), &props);

		if (tiling_ == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) return candidates.at(i);
		else if (tiling_ == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) return candidates.at(i);

	}

#ifndef NDEBUG
	Logger::log_exception("Failed to find supported format out of user-defined formats for image at: ", get_address(this), "!");
#endif

	// return error case
	return VK_FORMAT_MAX_ENUM;
}

} // namespace vulkan

} // namespace lyra
