/*************************
 * @file   command_pool.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief  A wrapper and manager around vulkan command pools
 * 
 * @date   2022-13-7
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <core/defines.h>
#include <core/rendering/vulkan/devices.h>
#include <core/logger.h>

#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief command pool
 */
struct VulkanCommandPool {
public:

	VulkanCommandPool::VulkanCommandPool() { }

	/**
	* @brief destructor of the command pool
	**/
	virtual ~VulkanCommandPool() noexcept {
		vkDestroyCommandPool(device->device(), _commandPool, nullptr);

		Logger::log_info("Successfully destroyed Vulkan command pool!");
	}

	/**
	 * @brief destroy the command pool
	 */
	void destroy() noexcept {
		this->~VulkanCommandPool();
	}

	VulkanCommandPool operator=(const VulkanCommandPool&) const noexcept = delete;

	/**
	 * @brief create a Vulkan command pool to allocate the command buffers
	 *
	 * @param device device
	 */
	void create(const VulkanDevice* const device);

	/**
	 * @brief reset the command buffer
	 */
	void reset() { lassert(vkResetCommandPool(device->device(), _commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT), "Failed to reset command pool!"); }

	/**
	 * @brief get the command pool
	 *
	 * @return const VkCommandPool&
	 */
	[[nodiscard]] const VkCommandPool& commandPool() const noexcept { return _commandPool; }

private:
	VkCommandPool _commandPool = VK_NULL_HANDLE;

	const VulkanDevice* device;
};

} // namespace lyra
