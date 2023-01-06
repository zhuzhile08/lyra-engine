#include <rendering/vulkan/vulkan_image.h>

#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/command_buffer.h>

#include <core/application.h>

namespace lyra {

namespace vulkan {

Image::~Image() {
	// destroy the image view
	vkDestroyImageView(Application::renderSystem.device.device(), m_view, nullptr);
	// destroy the image
	vkDestroyImage(Application::renderSystem.device.device(), m_image, nullptr);
}

void Image::create_view(const VkFormat& format, const VkImageSubresourceRange& subresourceRange, const VkImageViewType& viewType, const VkComponentMapping& colorComponents) {
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
	vassert(vkCreateImageView(Application::renderSystem.device.device(), &createInfo, nullptr, &m_view), "create Vulkan image views");
}

void Image::transition_layout(
	const VkImageLayout& oldLayout,
	const VkImageLayout& newLayout,
	const VkFormat& format,
	const VkImageSubresourceRange& subresourceRange
) const {
	// get a command buffer for setting up memory barrier
	CommandBuffer cmdBuff(Application::renderSystem.commandBuffers);
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
	else log().exception("Invalid image layout transition was requested whilst transitioning an image layout at: ", get_address(this));

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
	cmdBuff.submitQueue(Application::renderSystem.device.graphicsQueue().queue);
	// reset the command buffer
	cmdBuff.reset();
}

VkFormat Image::get_best_format(const std::vector<VkFormat>& candidates, const VkFormatFeatureFlags& features, const VkImageTiling& tiling) const {
	// check which tiling mode to use
	VkImageTiling finalTilingMode = (tiling == VK_IMAGE_TILING_MAX_ENUM) ? m_tiling : tiling;

	// check which of the canditates is the best choice
	for (uint32 i = 0; i < candidates.size(); i++) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(Application::renderSystem.device.physicalDevice(), candidates.at(i), &props);

		if (finalTilingMode == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) return candidates.at(i);
		else if (finalTilingMode == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) return candidates.at(i);
	}

	log().exception("Failed to find supported format out of user defined formats for image at: ", get_address(this), "!");
	// return error case
	return VK_FORMAT_MAX_ENUM;
}

void Image::copy_from_buffer(const vulkan::GPUBuffer* stagingBuffer, const VkExtent3D& extent, const uint32 layerCount) {
	// temporary command buffer for copying
	vulkan::CommandBuffer cmdBuff(Application::renderSystem.commandBuffers);
	// begin recording
	cmdBuff.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// copy image in buffer to the image
	VkBufferImageCopy imageCopy {
		0,
		0,
		0,
		{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, layerCount},
		{0, 0, 0},
		extent
	};
	cmdBuff.copyBufferToImage(stagingBuffer->buffer(), m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageCopy);

	// end recording
	cmdBuff.end();
	// submit queues after recording
	cmdBuff.submitQueue(Application::renderSystem.device.graphicsQueue().queue);
}

} // namespace vulkan

} // namespace lyra
