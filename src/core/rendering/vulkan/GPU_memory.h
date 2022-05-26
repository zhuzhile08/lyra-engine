/*************************
 * @file GPUMemory.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper around VMA GPU memory allocations

 * @date 2022-03-13
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief wrapper around the VMA GPU allocations
 */
struct VulkanGPUMemory {
	VulkanGPUMemory();

	VulkanGPUMemory operator=(const VulkanGPUMemory&) const noexcept = delete;

	/**
	 * @brief get the creation information of the allocation
	 *
	 * @param usage usage of the memory
	 * @param requiredFlags required memory flags
	 * 
	 * @return const VmaAllocationCreateInfo
	 */
	[[nodiscard]] const VmaAllocationCreateInfo get_alloc_create_info(const VmaMemoryUsage usage, const VkMemoryPropertyFlags requiredFlags = 0) const noexcept {
		return {
			0,
			usage,
			requiredFlags,
			0,
			0,
			0,
			nullptr,
			0
		}; // the rest is absolutely useless
	}

	VmaAllocation _memory = VK_NULL_HANDLE;
};

} // namespace lyra
