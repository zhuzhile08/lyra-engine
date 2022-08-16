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

#include <core/decl.h>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief wrapper around the VMA GPU allocations
 */
struct VulkanGPUMemory {
	VulkanGPUMemory() { };

	/**
	 * @brief destructor of the memory
	 */
	virtual ~VulkanGPUMemory();

	/**
	 * @brief manually destroy the memory
	 */
	void destroy() {
		this->~VulkanGPUMemory();
	}

	VulkanGPUMemory operator=(const VulkanGPUMemory&) const noexcept = delete;

	/**
	 * @brief get the creation information of the allocation
	 *
	 * @param usage usage of the memory
	 * @param requiredFlags required memory flags
	 * 
	 * @return const VmaAllocationCreateInfo
	 */
	NODISCARD const VmaAllocationCreateInfo get_alloc_create_info(const VmaMemoryUsage usage, const VkMemoryPropertyFlags requiredFlags = 0) noexcept;

	VmaAllocation _memory = VK_NULL_HANDLE;
};

} // namespace lyra
