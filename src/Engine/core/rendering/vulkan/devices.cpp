#include <core/rendering/vulkan/devices.h>

namespace lyra {

VulkanDevice::VulkanDevice() {}

VulkanDevice::~VulkanDevice() {
	vkDestroyDevice(_device, nullptr);
	vmaDestroyAllocator(_allocator);

	LOG_INFO("Succesfully destroyed Vulkan device!");
}

void VulkanDevice::destroy() noexcept {
	this->~VulkanDevice();
}

void VulkanDevice::create(const VulkanInstance* instance) {
	LOG_INFO("Creating Vulkan device...");

	this->instance = instance;
	pick_physical_device();
	create_logical_device();
	create_allocator();

	LOG_INFO("Succesfully created Vulkan device and allocated GPU at ", GET_ADDRESS(this), "!", END_L);
}

void VulkanDevice::check_requested_extensions(const std::vector <VkExtensionProperties> extensions, const std::vector <string> requestedExtensions) const {
	// go through every requested extensions and see if they are available
	for (string extension : requestedExtensions) {
		bool found = false;
		LOG_INFO("Available device extensions:");

			for (const auto& extensionProperties : extensions) {
				LOG_DEBUG(TAB, extensionProperties.extensionName);
					if (strcmp(extension, extensionProperties.extensionName) == 0) {
						found = true;
						break;
					}
			}

		if (!found) {
			LOG_EXEPTION("User required Vulkan extensions weren't found!", extension);
		}
	}
}

void VulkanDevice::find_family_index(VulkanQueueFamily* queue, const VkPhysicalDevice device) noexcept {
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

void VulkanDevice::create_queue(VulkanQueueFamily* queue) noexcept {
	vkGetDeviceQueue(_device, queue->familyIndex, 0, &queue->queue);
}

void VulkanDevice::pick_physical_device() {
	// get all devices
	uint32 deviceCount = 0;
	if (vkEnumeratePhysicalDevices(instance->instance(), &deviceCount, nullptr) != VK_SUCCESS) LOG_EXEPTION("Failed to find any Vulkan auitable GPUs!");
		std::vector <VkPhysicalDevice> devices(deviceCount);             // just put this in here cuz I was lazy
	vkEnumeratePhysicalDevices(instance->instance(), &deviceCount, devices.data());

	// a ordered map with every GPU. The one with the highest score is the one that is going to be the used GPU
	std::multimap <int, VkPhysicalDevice> possibleDevices;

	int i = 0;
	for (auto& device : devices) {
		LOG_INFO("GPU " + std::to_string(i + 1) + ": ");
		rate_physical_device(device, possibleDevices);
		i++;
	}

	if (possibleDevices.begin()->first <= 0) {
		LOG_EXEPTION("Failed to find GPU with enough features");
	}

	_physicalDevice = possibleDevices.begin()->second;
}

void VulkanDevice::rate_physical_device(const VkPhysicalDevice device, std::multimap <int, VkPhysicalDevice>& map) {
	// get the available extensions
	uint32 availableExtensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &availableExtensionCount, nullptr);
	std::vector <VkExtensionProperties> availableExtensions(availableExtensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &availableExtensionCount, availableExtensions.data());

	int score = 1;

	// get some device properties and features
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(device, &features);

	check_requested_extensions(availableExtensions, requested_device_extensions);
	find_family_index(&_graphicsQueue, device);
	find_family_index(&_presentQueue, device);

	// some required features. If not available, make the GPU unavailable
	if (!features.geometryShader && !features.samplerAnisotropy) {
		score = 0;
		LOG_WARNING("GPU does not have some required features!");
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
	std::vector <VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32> queueFamilies = { _graphicsQueue.familyIndex, _presentQueue.familyIndex };

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

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo                         createInfo{
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
	if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS) LOG_EXEPTION("Failed to create logical device!");

	create_queue(&_graphicsQueue);
	create_queue(&_presentQueue);
}

void VulkanDevice::create_allocator() {
	// creation info
	VmaAllocatorCreateInfo createInfo = {
		0,
		_physicalDevice,
		_device,
		0,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		instance->instance(),
		VK_API_VERSION_1_2
	};

	// create the allocator
	if (vmaCreateAllocator(&createInfo, &_allocator) != VK_SUCCESS) LOG_EXEPTION("Failed to create VMA memory allocator!");
}

void VulkanDevice::wait() const {
	vkDeviceWaitIdle(_device);
}

} // namespace lyra