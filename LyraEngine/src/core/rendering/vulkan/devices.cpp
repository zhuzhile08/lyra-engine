#include <core/rendering/vulkan/devices.h>

#include <core/context.h>
#include <core/rendering/window.h>

namespace lyra {

VulkanDevice::VulkanDevice() {
	Logger::log_info("Creating Vulkan device...");

	create_instance();
	pick_physical_device();
	create_logical_device();
	create_allocator();

	Logger::log_info("Successfully created Vulkan device and allocated GPU at ", get_address(this), "!", Logger::end_l());
}

VulkanDevice::~VulkanDevice() {
	vmaDestroyAllocator(_allocator);
	vkDestroyDevice(_device, nullptr);
	vkDestroyInstance(_instance, nullptr);

	Logger::log_info("Successfully destroyed Vulkan device!");
}

void VulkanDevice::destroy() noexcept {
	this->~VulkanDevice();
}

void VulkanDevice::check_requested_extensions(const std::vector <VkExtensionProperties> extensions, const std::vector <const char*> requestedExtensions) const {
	// go through every requested extensions and see if they are available
	for (uint32 i = 0; i < requestedExtensions.size(); i++) {
		bool found = false;
		Logger::log_info("Available device extensions:");

			for (uint32 j = 0; j < extensions.size(); j++) {
				Logger::log_debug(Logger::tab(), extensions.at(j).extensionName);
					if (strcmp(requestedExtensions.at(i), extensions.at(j).extensionName) == 0) {
						found = true;
						break;
					}
			}

		lassert(found, "User required Vulkan extensions weren't found!", requestedExtensions.at(i));
	}
}

void VulkanDevice::check_requested_validation_layers(const std::vector <VkLayerProperties>& layers, const std::vector <const char*>& requestedLayers) const {
	// go through every requested layers and see if they are available
	for (uint32 i = 0; i < requestedLayers.size(); i++) {
		bool found = false;
		Logger::log_info("Available layers:");

		for (uint32 j = 0; j < layers.size(); j++) {
			Logger::log_debug(Logger::tab(), layers.at(j).layerName, layers.at(j).description);
			if (strcmp(requestedLayers.at(i), layers.at(j).layerName) == 0) {
				found = true;
				break;
			}
		}

		lassert(found, "User required Vulkan validation layer wasn't found: ", requestedLayers.at(i));
	}
}

void VulkanDevice::find_family_index(VulkanQueueFamily* const queue, const VkPhysicalDevice device) noexcept {
	uint32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (uint32 i = 0; i < queueFamilies.size(); i++) {
		if (queueFamilies.at(i).queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queue->familyIndex = i;
			break;
		}
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

void VulkanDevice::create_queue(VulkanQueueFamily* const queue) noexcept {
	vkGetDeviceQueue(_device, queue->familyIndex, 0, &queue->queue);
}

void VulkanDevice::create_instance() {
	// check if requested validation layers are available
#ifdef _DEBUG
	uint32 availableLayerCount = 0;
	vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
	std::vector <VkLayerProperties> availableLayers(availableLayerCount);
	vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

	check_requested_validation_layers(availableLayers, Settings::Debug::requestedValidationLayers);
#endif
	// get all extensions
	uint32 SDLExtensionCount = 0;

	lassert(SDL_Vulkan_GetInstanceExtensions(Context::get()->window()->get(), &SDLExtensionCount, nullptr), "Failed to get number of Vulkan instance extensions");
	const char** SDLExtensions = new const char* [SDLExtensionCount];
	lassert(SDL_Vulkan_GetInstanceExtensions(Context::get()->window()->get(), &SDLExtensionCount, SDLExtensions), "Failed to get Vulkan instance extensions");

	// define some info for the application that will be used in instance creation
	VkApplicationInfo appInfo{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		Settings::Window::title.c_str(),
		VK_MAKE_VERSION(0, 0, 1),
		"LyraEngine",
		VK_MAKE_VERSION(0, 7, 0),
		VK_API_VERSION_1_3
	};

	// defining some instance info
	VkInstanceCreateInfo createInfo{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0,
		&appInfo,
#ifdef _DEBUG
		static_cast<uint32>(Settings::Debug::requestedValidationLayers.size()),
		Settings::Debug::requestedValidationLayers.data(),
#else
		0,
		nullptr,
#endif
		SDLExtensionCount,
		SDLExtensions
	};

	// create the instance
	lassert(vkCreateInstance(&createInfo, nullptr, &_instance) == VK_SUCCESS, "Failed to create Vulkan instance");

	delete[] SDLExtensions;
}

void VulkanDevice::pick_physical_device() {
	// get all devices
	uint32 deviceCount = 0;
	lassert(vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr) == VK_SUCCESS, "Failed to find any Vulkan auitable GPUs!");
		std::vector <VkPhysicalDevice> devices(deviceCount);             // just put this in here cuz I was lazy
	vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

	// a ordered map with every GPU. The one with the highest score is the one that is going to be the used GPU
	std::multimap <int, VkPhysicalDevice> possibleDevices;

	for (uint32 i = 0; i < devices.size(); i++) {
		Logger::log_info("GPU " + std::to_string(i + 1) + ": ");
		rate_physical_device(devices.at(i), possibleDevices);
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
		_instance,
		VK_API_VERSION_1_2
	};

	// create the allocator
	lassert(vmaCreateAllocator(&createInfo, &_allocator) == VK_SUCCESS, "Failed to create VMA memory allocator!");
}

} // namespace lyra