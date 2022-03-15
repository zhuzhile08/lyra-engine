#include <rendering/vulkan/devices.h>

namespace lyra {

VulkanDevice::VulkanDevice() {}

void VulkanDevice::destroy() {
	vkDestroyDevice(var.device, nullptr);
	vmaDestroyAllocator(var.allocator);

	delete instance;

	LOG_INFO("Succesfully destroyed Vulkan device!")
}

void VulkanDevice::create(VulkanInstance instance) {
	LOG_INFO("Creating Vulkan device...")

	this->instance = &instance;
	pick_physical_device();
	create_logical_device();
	create_allocator();

	LOG_INFO("Succesfully created Vulkan device and allocated GPU at ", GET_ADDRESS(this), "!", END_L)
}

void VulkanDevice::check_requested_extensions(const std::vector <VkExtensionProperties> extensions, const std::vector <str> requestedExtensions) const {
	// go through every requested extensions and see if they are available
	for (str extension : requestedExtensions) {
		bool found = false;
		LOG_INFO("Available device extensions:")

		for (const auto& extensionProperties : extensions) {
			LOG_DEBUG(TAB, extensionProperties.extensionName)
			if (strcmp(extension, extensionProperties.extensionName) == 0) {
				found = true;
				break;
			}
		}

		if (!found) {
			LOG_EXEPTION("User required Vulkan extensions weren't found", extension)
		}
	}
}

void VulkanDevice::find_family_index(VulkanQueueFamily *queue, const VkPhysicalDevice device) {
	uint32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queue->familyIndex = i;
			break;
		}

		i++;
	}
}

void VulkanDevice::create_queue(VulkanQueueFamily *queue) {
	vkGetDeviceQueue(var.device, queue->familyIndex, 0, &queue->queue);
}

void VulkanDevice::pick_physical_device() {
	// get all devices
	uint32 									deviceCount = 0;
	if(vkEnumeratePhysicalDevices(instance->get().instance, &deviceCount, nullptr) != VK_SUCCESS) LOG_DEBUG("Failed to find any Vulkan auitable GPUs")
	std::vector <VkPhysicalDevice> 			devices(deviceCount);             // just put this in here cuz I was lazy
	vkEnumeratePhysicalDevices(instance->get().instance, &deviceCount, devices.data());

	// a ordered map with every GPU. The one with the highest score is the one that is going to be the used GPU
	std::multimap <int, VkPhysicalDevice>	possibleDevices;

	int i = 0;
	for (auto& device : devices) {
		LOG_INFO("GPU " + std::to_string(i + 1) + ": ")
		rate_physical_device(device, possibleDevices);
		i++;
	}

	if (possibleDevices.begin()->first <= 0) {
		LOG_EXEPTION("Failed to find GPU with enough features")
	}

	var.physicalDevice = possibleDevices.begin()->second;
}

void VulkanDevice::rate_physical_device(const VkPhysicalDevice device, std::multimap <int, VkPhysicalDevice> &map) {
	// get the available extensions
	uint32 								availableExtensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &availableExtensionCount, nullptr);
	std::vector <VkExtensionProperties> availableExtensions(availableExtensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &availableExtensionCount, availableExtensions.data());

	int score = 1;

	// get some device properties and features
	VkPhysicalDeviceProperties 			properties;
	vkGetPhysicalDeviceProperties(device, &properties);
	VkPhysicalDeviceFeatures 			features;
	vkGetPhysicalDeviceFeatures(device, &features);

	check_requested_extensions(availableExtensions, requested_device_extensions);
	find_family_index(&var.graphicsQueue, device);
	find_family_index(&var.presentQueue, device);

	// some required features. If not available, make the GPU unavailable
	if (!features.geometryShader) {
		score = 0;
		LOG_WARNING("GPU does not have some required features");
	}

	// the actuall scoring system
	if (score > 0) {
		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += 10;
		}
	}
	LOG_INFO("Score: ", score, END_L);
	map.insert(std::make_pair(score, device));
}

void VulkanDevice::create_logical_device() {
	std::vector <VkDeviceQueueCreateInfo>   queueCreateInfos;
	std::set<uint32>                        queueFamilies = {var.graphicsQueue.familyIndex, var.presentQueue.familyIndex};

	float queuePriority = 1.0f;
	for (uint32 familyIndex : queueFamilies) {
		queueCreateInfos.push_back({
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			nullptr,
			0,
			familyIndex,
			1,
			&queuePriority
		});
	}

	// some info for device creation

	VkPhysicalDeviceFeatures 				deviceFeatures{};

	VkDeviceCreateInfo 						createInfo {
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32>(queueCreateInfos.size()),
		queueCreateInfos.data(),
#ifndef ndebug
		static_cast<uint32>(requested_validation_layers.size()),
		requested_validation_layers.data(),
#else
		0,
		nullptr,
#endif
		static_cast<uint32>(requested_device_extensions.size()),
		requested_device_extensions.data(),
		&deviceFeatures
	};

	// create the device and retrieve the graphics and presentation queue handles
	if(vkCreateDevice(var.physicalDevice, &createInfo, nullptr, &var.device) != VK_SUCCESS) LOG_EXEPTION("Failed to create logical device")

	create_queue(&var.graphicsQueue);
	create_queue(&var.presentQueue);
}

void VulkanDevice::create_allocator() {
	// creation info
	VmaAllocatorCreateInfo createInfo = {
		0,
		var.physicalDevice,
		var.device,
		0,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		instance->get().instance,
		VK_API_VERSION_1_2
	};

	// create the allocator
	vmaCreateAllocator(&createInfo, &var.allocator);
}

void VulkanDevice::map_memory(const VkDeviceMemory memory, const VkDeviceSize size, void *mapped) const {
	vkMapMemory(var.device, memory, 0, size, 0, &mapped);
}

void VulkanDevice::unmap_memory(const VkDeviceMemory memory) const {
	vkUnmapMemory(var.device, memory);
}

void VulkanDevice::wait() const {
	vkDeviceWaitIdle(var.device);
}

uint32 VulkanDevice::find_memory_type(const uint32 typeFilter, const VkMemoryPropertyFlags properties) const {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(var.physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
            return true;
        }
    }

    LOG_EXEPTION("Failed to find suitable memory type");
    return INT32_MAX;
}

VulkanDevice::Variables VulkanDevice::get() const {
	return var;
}

} // namespace lyra