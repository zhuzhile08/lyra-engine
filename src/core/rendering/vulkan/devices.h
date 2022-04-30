/*************************
 * @file devices.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a wrapper around the Vulkan devices
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/defines.h>
#include <math/math.h>
#include <core/rendering/vulkan/instance.h>
#include <core/logger.h>

#include <vector>
#include <set>
#include <map>

#include <SDL.h>
#include <vk_mem_alloc.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief Vulkan physical and logical devices
 */
struct VulkanDevice {
private:
	friend class Context;

	/**
	 * @brief queue families
	 */
	struct VulkanQueueFamily {
	public:
		VkQueue queue = VK_NULL_HANDLE;
		uint32  familyIndex = 0;
	};

public:
	VulkanDevice();

	/**
	* @brief destructor of the device
	**/
	virtual ~VulkanDevice() noexcept;

	/**
	 * @brief destroy the device
	 */
	void destroy() noexcept;

	/**
	 * @brief No copy constructors?
	 * @param ⠀⣞⢽⢪⢣⢣⢣⢫⡺⡵⣝⡮⣗⢷⢽⢽⢽⣮⡷⡽⣜⣜⢮⢺⣜⢷⢽⢝⡽⣝
	 * @param ⠸⡸⠜⠕⠕⠁⢁⢇⢏⢽⢺⣪⡳⡝⣎⣏⢯⢞⡿⣟⣷⣳⢯⡷⣽⢽⢯⣳⣫⠇
	 * @param ⠀⠀⢀⢀⢄⢬⢪⡪⡎⣆⡈⠚⠜⠕⠇⠗⠝⢕⢯⢫⣞⣯⣿⣻⡽⣏⢗⣗⠏⠀
	 * @param ⠀⠪⡪⡪⣪⢪⢺⢸⢢⢓⢆⢤⢀⠀⠀⠀⠀⠈⢊⢞⡾⣿⡯⣏⢮⠷⠁⠀⠀
	 * @param ⠀⠀⠀⠈⠊⠆⡃⠕⢕⢇⢇⢇⢇⢇⢏⢎⢎⢆⢄⠀⢑⣽⣿⢝⠲⠉⠀⠀⠀⠀
	 * @param ⠀⠀⠀⠀⠀⡿⠂⠠⠀⡇⢇⠕⢈⣀⠀⠁⠡⠣⡣⡫⣂⣿⠯⢪⠰⠂⠀⠀⠀⠀
	 * @param ⠀⠀⠀⠀⡦⡙⡂⢀⢤⢣⠣⡈⣾⡃⠠⠄⠀⡄⢱⣌⣶⢏⢊⠂⠀⠀⠀⠀⠀⠀
	 * @param ⠀⠀⠀⠀⢝⡲⣜⡮⡏⢎⢌⢂⠙⠢⠐⢀⢘⢵⣽⣿⡿⠁⠁⠀⠀⠀⠀⠀⠀⠀
	 * @param ⠀⠀⠀⠀⠨⣺⡺⡕⡕⡱⡑⡆⡕⡅⡕⡜⡼⢽⡻⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
	 * @param ⠀⠀⠀⠀⣼⣳⣫⣾⣵⣗⡵⡱⡡⢣⢑⢕⢜⢕⡝⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
	 * @param ⠀⠀⠀⣴⣿⣾⣿⣿⣿⡿⡽⡑⢌⠪⡢⡣⣣⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
	 * @param ⠀⠀⠀⡟⡾⣿⢿⢿⢵⣽⣾⣼⣘⢸⢸⣞⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
	 * @param⠀⠀⠀⠀⠁⠇⠡⠩⡫⢿⣝⡻⡮⣒⢽⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
	 */
	VulkanDevice operator=(const VulkanDevice&) const noexcept = delete;

	/**
	 * @brief create the devices
	 *
	 * @param instance instance
	 */
	void create(const VulkanInstance* const instance);

	/**
	 * @brief wait for the logical device to finish with whatever operations are still going on
	 */
	void wait() const;

	/**
	 * @brief get the GPU
	 *
	 * @return VkPhysicalDevice
	 */
	[[nodiscard]] const VkPhysicalDevice physicalDevice() const noexcept { return _physicalDevice;  }
	/**
	 * @brief get the logical device
	 * 
	 * @return const VkLogicalDevice
	 */
	[[nodiscard]] const VkDevice device() const noexcept { return _device; }
	/**
	 * @brief get the graphics queue
	 * 
	 * @return const lyra::VulkanQueueFamily
	 */
	[[nodiscard]] const VulkanQueueFamily graphicsQueue() const noexcept { return _graphicsQueue; }
	/**
	 * @brief get the presentation queue
	 *
	 * @return const lyra::VulkanQueueFamily
	 */
	[[nodiscard]] const VulkanQueueFamily presentQueue() const noexcept { return _presentQueue; }
	/**
	 * @brief get the VMA memory allocator
	 *
	 * @return const VmaAllocator
	 */
	[[nodiscard]] const VmaAllocator allocator() const noexcept { return _allocator; }

private:
	VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
	VkDevice _device = VK_NULL_HANDLE;

	VulkanQueueFamily _graphicsQueue;
	VulkanQueueFamily _presentQueue;

	VmaAllocator _allocator = VK_NULL_HANDLE;

	const VulkanInstance* instance;

	/**
	 * @brief find the family index of a queues
	 *
	 * @param queue the queue to find the family index of
	 * @param device device to find the family index of the queue of
	 * @return VulkanQueueFamily
	 */
	void find_family_index(VulkanQueueFamily* const queue, const VkPhysicalDevice device) noexcept;
	/**
	 * @brief create a Vulkan queue
	 *
	 * @return VulkanQueueFamily
	 */
	void create_queue(VulkanQueueFamily* const queue) noexcept;

	/**
	 * @brief rate a physical device by its features
	 *
	 * @param device the device to rate
	 * @param map a map containing all the physical devices and their scores
	 */
	void rate_physical_device(const VkPhysicalDevice device, std::multimap <int, VkPhysicalDevice>& map);
	/**
	 * @brief check requested Vulkan device extensions
	 *
	 * @param extensions the available extensions
	 * @param requestedExtensions the requested extensions
	 */
	void check_requested_extensions(const std::vector <VkExtensionProperties> extensions, const std::vector <string> requestedExtensions) const;

	/**
	 * @brief select a physical device from the available ones
	 */
	void pick_physical_device();
	/**
	 * @brief create a logical device
	 */
	void create_logical_device();
	/**
	 * @brief create the VMA memoryW allocator
	 */
	void create_allocator();
};

} // namespace lyra
