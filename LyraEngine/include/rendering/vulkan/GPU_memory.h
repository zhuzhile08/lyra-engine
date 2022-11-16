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

#ifdef NDEBUG
#include <core/application.h>
#include <rendering/vulkan/devices.h>
#endif


namespace lyra {

namespace vulkan {

/**
 * @brief wrapper around the VMA GPU allocations
 */
struct GPUMemory {
	constexpr GPUMemory() = default;

	/**
	 * @brief destructor of the memory
	 */
	#ifndef NDEBUG
	virtual ~GPUMemory();
	#else
	virtual ~GPUMemory() {
		Application::renderSystem()->device()->freeMemory(m_memory);
	}
	#endif
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
	 * @return constexpr VmaAllocationCreateInfo
	 */
	NODISCARD constexpr VmaAllocationCreateInfo get_alloc_create_info(const VmaMemoryUsage usage, const VkMemoryPropertyFlags requiredFlags = 0) noexcept {
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

	VmaAllocation m_memory;
};

} // namespace vulkan

} // namespace lyra
