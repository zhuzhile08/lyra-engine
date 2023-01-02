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

#include <lyra.h>

#include <vector>
#include <vulkan/vulkan.h>

#include <rendering/vulkan/GPU_buffer.h>

namespace lyra {

namespace vulkan {

/**
 * @brief wrapper around vulkan images
 */
struct Image {
	/**
	* @brief destructor of the image
	**/
	virtual ~Image();
	/**
	 * @brief destroy the image
	 */
	void destroy() {
		this->~Image();
	}

	Image operator=(const Image&) const noexcept = delete;

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
	 * @return constexpr VkImageCreateInfo
	 */
	NODISCARD constexpr VkImageCreateInfo get_image_create_info(
		const VkFormat& format,
		const VkExtent3D& extent,
		const VkImageUsageFlags& usage,
		const uint32& mipLevels = 1,
		const VkImageType& imageType = VK_IMAGE_TYPE_2D,
		const uint32& arrayLayers = 1,
		const VkImageCreateFlags& flags = 0,
		const VkSampleCountFlagBits& samples = VK_SAMPLE_COUNT_1_BIT,
		const VkImageTiling& tiling = VK_IMAGE_TILING_OPTIMAL
	) noexcept {
		m_tiling = tiling;

		return {
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			nullptr,
			flags,
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
		const VkFormat& format,
		const VkImageSubresourceRange& subresourceRange,
		const VkImageViewType& viewType = VK_IMAGE_VIEW_TYPE_2D,
		const VkComponentMapping& colorComponents = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY }
	);

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
	 * @return constexpr VkImageMemoryBarrier
	*/
	NODISCARD constexpr VkImageMemoryBarrier get_image_memory_barrier(
		const VkAccessFlags& srcAccessMask,
		const VkAccessFlags& dstAccessMask,
		const VkImageLayout& srcLayout,
		const VkImageLayout& dstLayout,
		const VkImageSubresourceRange& subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1 , 0, 1 },
		const uint32& srcQueueFamily = VK_QUEUE_FAMILY_IGNORED,
		const uint32& dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
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
			m_image,
			subresourceRange
		};
	}

	/**
	 * @brief transition the image layout to an another one
	 * 
	 * @param oldLayout old layout
	 * @param newLayout new layout
	 * @param format format of the image
	 * @param subresourceRange some data about the image
	*/
	void transition_layout(
		const VkImageLayout& oldLayout,
		const VkImageLayout& newLayout,
		const VkFormat& format,
		const VkImageSubresourceRange& subresourceRange
	) const;
	

	/**
	 * @brief get the best format out of a vector of requested ones for a certain situation
	 *
	 * @param candidates all candidates
	 * @param features what type the image is
	 * @param tiling tiling mode of the image
	 *
	 * @return VkFormat
	*/
	NODISCARD VkFormat get_best_format(const std::vector<VkFormat>& candidates, const VkFormatFeatureFlags& features, const VkImageTiling& tiling = VK_IMAGE_TILING_MAX_ENUM) const;

	/**
	 * @brief copy the contents of a buffer into the image
	 * 
	 * @param stagingBuffer staging buffer containing the data
	 * @param extent image dimensions
	 * @param layerCount "number" of "images" in the image, only reserved for cubemaps
	 */
	void copy_from_buffer(const vulkan::GPUBuffer* stagingBuffer, const VkExtent3D& extent, const uint32 layerCount = 1);

	VkImage m_image;
	VkImageView m_view;

private:
	VkImageTiling m_tiling = VK_IMAGE_TILING_MAX_ENUM;
};

} // namespace vulkan

} // namespace lyra
