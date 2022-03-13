/*************************
 * @file devices.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief a wrapper around the Vulkan devices
 * 
 * @date 2022-02-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <core/defines.h>
#include <math/math.h>
#include <rendering/vulkan/instance.h>
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
	/**
	 * @brief queue families
	 */
	struct 				VulkanQueueFamily {
	public:
		VkQueue 		queue;
		uint32  		familyIndex = 0;
	};

	/**
	 * @brief struct containing all the variables
	 */
	struct 				Variables {
		VkPhysicalDevice 	physicalDevice;
		VkDevice         	device;	

		VulkanQueueFamily 	graphicsQueue;
		VulkanQueueFamily 	presentQueue;

		VmaAllocator 		allocator;
	};
	
public:
	VulkanDevice();

	/**
	 * @brief destroy the VulkanDevice object
	 */
	void 				destroy();

	/**
	 * @brief create the devices
	 * 
	 * @param instance instance
	 */
	void 				create(VulkanInstance instance);

	/**
	 * @brief find a block of memory that has requested settings
	 * 
	 * @param typeFilter types of memory that are usable
	 * @param properties the properties that the memory needs to have
	 */
	uint32 				find_memory_type(const uint32 typeFilter, const VkMemoryPropertyFlags properties) const;

	/**
	 * @brief map GPU memory
	 * 
	 * @param memory memory to map
	 * @param size size of the memory
	 * @param mapped currently mapped memory
	 */
	void 				map_memory(const VkDeviceMemory memory, const VkDeviceSize size, void *mapped) const;
	/**
	 * @brief unmap GPU memory
	 * 
	 * @param memory memory to unmap
	 */
	void 				unmap_memory(const VkDeviceMemory memory) const;

	/**
	 * @brief wait for device to finish
	 */
	void 				wait() const;

	/**
	 * @brief Get the physical device
	 * 
	 * @return VkPhysicalDevice 
	 */
	Variables 			get() const;

private:
	Variables 			var;

	VulkanInstance* 	instance;

	/**
	 * @brief find the family index of a queues
	 * 
	 * @param queue the queue to find the family index of
	 * @param device device to find the family index of the queue of
	 * @return VulkanQueueFamily 
	 */
	void 				find_family_index(VulkanQueueFamily *queue, const VkPhysicalDevice device);
	/**
	 * @brief create a Vulkan queue
	 * 
	 * @return VulkanQueueFamily 
	 */
	void 				create_queue(VulkanQueueFamily *queue);

	/**
	 * @brief rate a physical device by its features
	 * 
	 * @param device the device to rate
	 * @param map a map containing all the physical devices and their scores
	 */
	void 				rate_physical_device(const VkPhysicalDevice device, std::multimap <int, VkPhysicalDevice> &map);
	/**
	 * @brief check requested Vulkan device extensions
	 * 
	 * @param extensions the available extensions
	 * @param requestedExtensions the requested extensions
	 */
	void 				check_requested_extensions(const std::vector <VkExtensionProperties> extensions, const std::vector <str> requestedExtensions) const;

	/**
	 * @brief select a physical device from the available ones
	 */
	void 				pick_physical_device();
	/**
	 * @brief create a logical device
	 */
	void 				create_logical_device();
	/**
	 * @brief create the VMA memory allocator
	 */
	void 				create_allocator();
};

} // namespace lyra
