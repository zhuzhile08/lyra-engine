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
	 * @brief destroy the image and the image view
	 */
	void destroy_view() noexcept;

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
	 */
	VkImageCreateInfo	get_image_create_info(
		VkFormat format,
		VkExtent3D extent,
		VkImageUsageFlags usage,
		VkImageType imageType = VK_IMAGE_TYPE_2D,
		uint32 mipLevels = 1,
		uint32 arrayLayers = 1,
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
		VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL
	) noexcept;

	/**
	 * @brief create the image view only
	 *
	 * @param device device
	 * @param format format of the image
	 * @param subresourceRange describe what the purpose of the image is and other stuff like midmapping
	 * @param viewType how and in which dimension to treat the image
	 * @param colorComponents color modulation of the image
	 */
	void create_view(
		const VulkanDevice* device,
		VkFormat format,
		VkImageSubresourceRange subresourceRange,
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D,
		VkComponentMapping colorComponents = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY }
	);

	/**
	 * @brief transition the image layout to an another one
	 * 
	 * @param device device
	 * @param oldLayout old layout
	 * @param newLayout new layout
	 * @param format format of the image
	 * @param aspect purpose of the image
	 * @param commandPool command pool
	*/
	void transition_layout(VulkanDevice device, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkFormat format, const VkImageAspectFlags aspect, const VulkanCommandPool commandPool) const;

	VkImage _image = VK_NULL_HANDLE;
	VkImageView _view = VK_NULL_HANDLE;

private:
	const VulkanDevice* device;
	VkImageTiling _tiling = VK_IMAGE_TILING_MAX_ENUM;

protected:
	/**
	 * @brief get the best format out of a vector of requested ones for a certain situation
	 * 
	 * @param device device (ignore the obviously bad architecture here)
	 * @param candidates all candidates
	 * @param features what type the image is
	 * @param tiling tiling mode of the image
	 * 
	 * @return const VkFormat
	*/
	const VkFormat get_best_format(VulkanDevice device, const std::vector<VkFormat> candidates, VkFormatFeatureFlags features, VkImageTiling tiling = VK_IMAGE_TILING_MAX_ENUM) const;
};

} // namespace lyra