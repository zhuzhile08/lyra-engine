/*************************
 * @file vulkan_image.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper around vulkan images. Do not mistake for textures

 * @date 2022-03-13
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/defines.h>
#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/instance.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/logger.h>

#include <vector>

#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief wrapper around vulkan images
 */
struct VulkanImage {
	VulkanImage();

	/**
	* @brief destructor of the image
	**/
	virtual ~VulkanImage() noexcept {
		vkDestroyImageView(device->device(), _view, nullptr);
		vkDestroyImage(device->device(), _image, nullptr);

		Logger::log_debug(Logger::tab(), "Successfully destroyed Vulkan images!");
	}

	/**
	 * @brief destroy the image
	 */
	virtual void destroy() noexcept {
		this->~VulkanImage();
	}

	VulkanImage operator=(const VulkanImage&) const noexcept = delete;

	/**
	 * @brief create the image and image view
	 *
	 * @param format define how the data is stored in the image
	 * @param extent size of the image
	 * @param usage how or as what will be the image used
	 * @param imageType the dimension to treat the image
	 * @param mipLevels mipmapping stages of the image
	 * @param arrayLayers layers of the image
	 * @param samples configure multi sample anti-aliasing
	 * @param tiling how to store the image in the GPU memory
	 * 
	 * @return const VkImageCreateInfo
	 */
	[[nodiscard]] const VkImageCreateInfo get_image_create_info(
		const VkFormat format,
		const VkExtent3D extent,
		const VkImageUsageFlags usage,
		const uint32 mipLevels = 1,
		const VkImageType imageType = VK_IMAGE_TYPE_2D,
		const uint32 arrayLayers = 1,
		const VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
		const VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL
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

	/**
	 * @brief create the image view only
	 *
	 * @param format format of the image
	 * @param subresourceRange describe what the purpose of the image is and other stuff like mipmapping
	 * @param viewType how and in which dimension to treat the image
	 * @param colorComponents color modulation of the image
	 */
	void create_view(
		const VkFormat format,
		const VkImageSubresourceRange subresourceRange,
		const VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D,
		const VkComponentMapping colorComponents = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY }
	) {
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
		if (vkCreateImageView(device->device(), &createInfo, nullptr, &_view) != VK_SUCCESS) Logger::log_exception("Failed to create Vulkan image views");

		Logger::log_debug(Logger::tab(), "Successfully created Vulkan image view at ", get_address(this), "!");
	}

	/**
	 * @brief create the image view only
	 *
	 * @param device device, if not already defined
	 * @param format format of the image
	 * @param subresourceRange describe what the purpose of the image is and other stuff like mipmapping
	 * @param viewType how and in which dimension to treat the image
	 * @param colorComponents color modulation of the image
	 */
	void create_view(
		const VulkanDevice* const device,
		const VkFormat format,
		const VkImageSubresourceRange subresourceRange,
		const VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D,
		const VkComponentMapping colorComponents = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY }
	) {
		this->device = device;

		create_view(format, subresourceRange, viewType, colorComponents);
	}

	/**
	 * @brief return a memory barrier for this image
	 *
	 * @param srcAccessMask the original accessability for the image
	 * @param dstAccessMask the new accessability to transition to
	 * @param srcLayout the original layout of the image
	 * @param dstLayout the layout to transition to
	 * @param srcQueueFamily the original queue family of the image
	 * @param dstQueueFamily the queue family to transfer ownership to
	 * @param subresourceRange some data about the image
	 *
	 * @return const VkImageMemoryBarrier
	*/
	[[nodiscard]] const VkImageMemoryBarrier get_image_memory_barrier(
		const VkAccessFlags srcAccessMask,
		const VkAccessFlags dstAccessMask,
		const VkImageLayout srcLayout,
		const VkImageLayout dstLayout,
		const VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1 , 0, 1 },
		const uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED,
		const uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
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

	/**
	 * @brief transition the image layout to an another one
	 *
	 * @param device device, if not already defined
	 * @param commandPool command pool
	 * @param oldLayout old layout
	 * @param newLayout new layout
	 * @param format format of the image
	 * @param aspect purpose of the image
	*/
	void transition_layout(
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

	/**
	 * @brief transition the image layout to an another one
	 * 
	 * @param commandPool command pool
	 * @param oldLayout old layout
	 * @param newLayout new layout
	 * @param format format of the image
	 * @param aspect purpose of the image
	*/
	void transition_layout(
		const VulkanCommandPool* const commandPool,
		const VkImageLayout oldLayout,
		const VkImageLayout newLayout,
		const VkFormat format,
		const VkImageSubresourceRange subresourceRange
	) const;

	VkImage _image = VK_NULL_HANDLE;
	VkImageView _view = VK_NULL_HANDLE;

private:
	const VulkanDevice* device;
	VkImageTiling _tiling = VK_IMAGE_TILING_MAX_ENUM;

protected:
	/**
	 * @brief get the best format out of a vector of requested ones for a certain situation
	 *
	 * @param candidates all candidates
	 * @param features what type the image is
	 * @param tiling tiling mode of the image
	 *
	 * @return const VkFormat
	*/
	[[nodiscard]] const VkFormat get_best_format(const std::vector<VkFormat> candidates, const VkFormatFeatureFlags features, const VkImageTiling tiling = VK_IMAGE_TILING_MAX_ENUM) const;

	/**
	 * @brief get the best format out of a vector of requested ones for a certain situation
	 *
	 * @param device device, if not already defined
	 * @param candidates all candidates
	 * @param features what type the image is
	 * @param tiling tiling mode of the image
	 *
	 * @return const VkFormat
	*/
	[[nodiscard]] const VkFormat get_best_format(const VulkanDevice* const device, const std::vector<VkFormat> candidates, const VkFormatFeatureFlags features, const VkImageTiling tiling = VK_IMAGE_TILING_MAX_ENUM) {
		this->device = device;

		return get_best_format(candidates, features, tiling);
	}
};

} // namespace lyra