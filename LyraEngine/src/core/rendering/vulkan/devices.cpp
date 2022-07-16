#include <core/rendering/vulkan/devices.h>

namespace lyra {

VulkanDevice::~VulkanDevice() {
	vmaDestroyAllocator(_allocator);
	vkDestroyDevice(_device, nullptr);

	Logger::log_info("Successfully destroyed Vulkan device!");
}

void VulkanDevice::destroy() noexcept {
	this->~VulkanDevice();
}

void VulkanDevice::create(const VulkanInstance* const instance) {
	Logger::log_info("Creating Vulkan device...");

	this->instance = instance;
	pick_physical_device();
	create_logical_device();
	create_allocator();

	Logger::log_info("Successfully created Vulkan device and allocated GPU at ", get_address(this), "!", Logger::end_l());
}

void VulkanDevice::check_requested_extensions(const std::vector <VkExtensionProperties> extensions, const std::vector <const char*> requestedExtensions) const {
	// go through every requested extensions and see if they are available
	for (const char* extension : requestedExtensions) {
		bool found = false;
		Logger::log_info("Available device extensions:");

			for (const auto& extensionProperties : extensions) {
				Logger::log_debug(Logger::tab(), extensionProperties.extensionName);
					if (strcmp(extension, extensionProperties.extensionName) == 0) {
						found = true;
						break;
					}
			}

		lassert(found, "User required Vulkan extensions weren't found!", extension);
	}
}

void VulkanDevice::find_family_index(VulkanQueueFamily* const queue, const VkPhysicalDevice device) noexcept {
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

void VulkanDevice::rate_physical_device(const VkPhysicalDevice& device, std::multimap <int, VkPhysicalDevice>& map) {
	// get the available extensions
	uint32 availableExtensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &availableExtensionCount, nullptr);
	std::vector <VkExtensionProperties> availableExtensions(availableExtensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &availableExtensionCount, availableExtensions.data());

	// get some device properties and features
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(device, &features);

	check_requested_extensions(availableExtensions, Settings::Debug::requestedDeviceExtensions);

	int score = 1;

	// some required features. If not available, make the GPU unavailable
#ifdef DRAW_INDIRECT
	if (!features.geometryShader || !features.multiDrawIndirect) {
		score = 0;
		Logger::log_warning("GPU does not have some required features!");
	}
#else
	if (!features.geometryShader) {
		score = 0;
		Logger::log_warning("GPU does not have some required features!");
	}
#endif

	Logger::log_info("Available device features and properties: ");

	// the actuall scoring system
	if (score > 0) {
		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) { // cpu type
			score += 10;
			Logger::log_debug(Logger::tab(), "Discrete GPU");
		} if (features.multiDrawIndirect) { // indirect drawing
			score += 6;
			Logger::log_debug(Logger::tab(), "Supports indirect drawing");
		} if (features.samplerAnisotropy) {
			score += 4;
			Logger::log_debug(Logger::tab(), "Supports anistropic filtering");
		}
	}
	Logger::log_info("Score: ", score, Logger::end_l());
	map.insert(std::make_pair(score, device));
}

void VulkanDevice::pick_physical_device() {
	// get all devices
	uint32 deviceCount = 0;
	lassert(vkEnumeratePhysicalDevices(instance->instance(), &deviceCount, nullptr) == VK_SUCCESS, "Failed to find any Vulkan auitable GPUs!");
		std::vector <VkPhysicalDevice> devices(deviceCount);             // just put this in here cuz I was lazy
	vkEnumeratePhysicalDevices(instance->instance(), &deviceCount, devices.data());

	// a ordered map with every GPU. The one with the highest score is the one that is going to be the used GPU
	std::multimap <int, VkPhysicalDevice> possibleDevices;

	int i = 0;
	for (auto& device : devices) {
		Logger::log_info("GPU " + std::to_string(i + 1) + ": ");
		rate_physical_device(device, possibleDevices);
		i++;
	}

	if (possibleDevices.begin()->first <= 0) {
		Logger::log_exception("Failed to find GPU with enough features");
	}

	_physicalDevice = possibleDevices.begin()->second;
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

	VkPhysicalDeviceFeatures deviceFeatures {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.sampleRateShading = VK_TRUE;

	VkDeviceCreateInfo createInfo {
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32>(queueCreateInfos.size()),
		queueCreateInfos.data(),
#ifdef _DEBUG
		static_cast<uint32>(Settings::Debug::requestedValidationLayers.size()),
		Settings::Debug::requestedValidationLayers.data(),
#else
		0,
		nullptr,
#endif
		static_cast<uint32>(Settings::Debug::requestedDeviceExtensions.size()),
		Settings::Debug::requestedDeviceExtensions.data(),
		&deviceFeatures
	};

	// create the device and retrieve the graphics and presentation queue handles
	lassert(vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) == VK_SUCCESS, "Failed to create logical device!");

	find_family_index(&_graphicsQueue, _physicalDevice);
	find_family_index(&_presentQueue, _physicalDevice);
	create_queue(&_graphicsQueue);
	create_queue(&_presentQueue);
}

void VulkanDevice::create_queue(VulkanQueueFamily* const queue) noexcept {
	vkGetDeviceQueue(_device, queue->familyIndex, 0, &queue->queue);
}

void VulkanDevice::create_allocator() {
	// creation info
	VmaAllocatorCreateInfo createInfo {
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
	lassert(vmaCreateAllocator(&createInfo, &_allocator) == VK_SUCCESS, "Failed to create VMA memory allocator!");
}

} // namespace lyra