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
#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/instance.h>
#include <core/logger.h>

#include <vector>

#include <vulkan/vulkan.h>

namespace lyra {

struct VulkanImage {
	VulkanImage();

	/**
	 * @brief destroy the image and the image view
	 */
	void            destroy();

	/**
	 * @brief create the image and image view
	 * 
	 * @param device device
	 * @param format define how the data is stored in the image
	 * @param extent size of the image
	 * @param usage how or as what will be the image used
	 * @param subresourceRange describe what the purpose of the image is and other stuff like midmapping
	 * @param imageType the dimension to treat the image
	 * @param mipLevels mipmapping stages of the image
	 * @param arrayLayers layers of the image
	 * @param samples configure multi sample anti-aliasing
	 * @param tiling how to store the image in the GPU memory
	 * @param viewType viewType how and in which dimension should the view to treat the image
	 * @param colorComponents color modulation of the image
	 */
	void            create(
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
	);

	/**
	 * @brief create the image view only
	 * 
	 * @param device device
	 * @param format format of the image
	 * @param subresourceRange describe what the purpose of the image is and other stuff like midmapping
	 * @param colorComponents color modulation of the image
	 * @param viewType how and in which dimension to treat the image
	 */
	void            create_view(
		VulkanDevice            device, 
		VkFormat                format, 
		VkImageSubresourceRange subresourceRange,
		VkImageViewType         viewType = VK_IMAGE_VIEW_TYPE_2D, 
		VkComponentMapping      colorComponents = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY}
	);

	VkImage 		image;
	VkImageView 	view;

private:
	VulkanDevice*   device;
};

} // namespace lyra
