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

#include <lyra.h>

#include <vulkan/vulkan.h>
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

#include <rendering/vulkan/vulkan_raii.h>

namespace lyra {

namespace vulkan {

/**
 * @brief wrapper around the VMA GPU allocations
 */
class GPUMemory {
public:
	virtual ~GPUMemory() = default;
	/**
	 * @brief manually destroy the memory
	 */
	void destroy() {
		this->~GPUMemory();
	}

	/**
	 * @brief get the creation information of the allocation
	 *
	 * @param usage usage of the memory
	 * @param requiredFlags required memory flags
	 * 
	 * @return constexpr VmaAllocationCreateInfo
	 */
	NODISCARD constexpr static VmaAllocationCreateInfo get_alloc_create_info(const VmaMemoryUsage usage, const VkMemoryPropertyFlags requiredFlags = 0) noexcept {
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

	/**
	 * @brief get the memory
	 * 
	 * @return constexpr lyra::vulkan::vma::Allocation&
	*/
	NODISCARD constexpr vma::Allocation& memory() noexcept { return m_memory; }
	/**
	 * @brief get the memory
	 * 
	 * @return constexpr const lyra::vulkan::vma::Allocation&
	*/
	NODISCARD constexpr const vma::Allocation& memory() const noexcept { return m_memory; }

protected:
	vma::Allocation m_memory;
};

} // namespace vulkan

} // namespace lyra
