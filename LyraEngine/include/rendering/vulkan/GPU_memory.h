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

#ifdef __APPLE__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#pragma clang diagnostic ignored "-Wdeprecated-copy"
#pragma clang diagnostic ignored "-Wignored-qualifiers"
#pragma clang diagnostic ignored "-Wswitch"
#include <vk_mem_alloc.h>
#pragma clang diagnostic pop
#else
#include <vk_mem_alloc.h>
#endif

#include <vulkan/vulkan.h>


namespace lyra {

namespace vulkan {

/**
 * @brief wrapper around the VMA GPU allocations
 */
struct GPUMemory {
	GPUMemory() { };

	/**
	 * @brief destructor of the memory
	 */
	virtual ~GPUMemory();

	/**
	 * @brief manually destroy the memory
	 */
	void destroy() {
		this->~GPUMemory();
	}

	GPUMemory operator=(const GPUMemory&) const noexcept = delete;

	/**
	 * @brief get the creation information of the allocation
	 *
	 * @param usage usage of the memory
	 * @param requiredFlags required memory flags
	 * 
	 * @return const VmaAllocationCreateInfo
	 */
	NODISCARD const VmaAllocationCreateInfo get_alloc_create_info(const VmaMemoryUsage usage, const VkMemoryPropertyFlags requiredFlags = 0) noexcept;

	VmaAllocation m_memory = VK_NULL_HANDLE;
};

} // namespace vulkan

} // namespace lyra
