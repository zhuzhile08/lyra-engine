#include <rendering/vulkan/devices.h>

#include <set>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <core/settings.h>

#include <rendering/window.h>
#include <core/application.h>

namespace lyra {

namespace vulkan {

Device::Device() {
	create_instance();
	pick_physical_device();
	create_logical_device();
	create_allocator();
}

void Device::check_requested_extensions(const std::vector <VkExtensionProperties> extensions, const std::vector <const char*> requestedExtensions) const {
	// go through every requested extensions and see if they are available
#ifndef NDEBUG
	log().info("Available device extensions:");
	for (const auto& extension : extensions) log().debug(log().tab(), extension.extensionName);
#endif
	for (const auto& requestedExtension : requestedExtensions) {
		bool found = false;
		for (const auto& extension : extensions) {
				if (strcmp(requestedExtension, extension.extensionName) == 0) {
					found = true;
					break;
				}
		}
		lassert(found, "User required Vulkan extensions weren't found!", requestedExtension);
	}
}

void Device::check_requested_validation_layers(const std::vector <VkLayerProperties>& layers, const std::vector <const char*>& requestedLayers) const {
	// go through every requested layers and see if they are available
	for (const auto& requestedLayer : requestedLayers) {
		bool found = false;
		log().info("Available layers:");

		for (const auto& layer : layers) {
			log().debug(log().tab(), layer.layerName, ": ", layer.description);
			if (strcmp(requestedLayer, layer.layerName) == 0) {
				found = true;
				break;
			}
		}

		lassert(found, "User required Vulkan validation layer wasn't found: ", requestedLayer);
	}
}

void Device::find_family_index(QueueFamily& queue, const VkPhysicalDevice device) noexcept {
	uint32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queue.familyIndex = (&queueFamily - &queueFamilies[0]);
			break;
		}
	}
}

void Device::rate_physical_device(const VkPhysicalDevice& device, std::multimap <int, VkPhysicalDevice>& map) {
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

	check_requested_extensions(availableExtensions, settings().debug.requestedDeviceExtensions);

	int score = 1;

	// some required features. If not available, make the GPU unavailable
#ifndef __APPLE__
#ifdef DRAW_INDIRECT
	if (!features.geometryShader || !features.multiDrawIndirect) {
		score = 0;
		log().warning("GPU does not have some required features!");
	}
#else
	if (!features.geometryShader) {
		score = 0;
		log().warning("GPU does not have some required features!");
	}
#endif
#endif

	log().info("Available device features and properties: ");

	// the actuall scoring system
	if (score > 0) {
		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) { // cpu type
			score += 10;
			log().debug(log().tab(), "Discrete GPU");
		} if (features.multiDrawIndirect) { // indirect drawing
			score += 6;
			log().debug(log().tab(), "Supports indirect drawing");
#ifndef DRAW_INDIRECT
			log().warning("Indirect draw calls are not enabled, but are supported. Please turn indirect drawing on if you want better performance.");
#endif
		} if (features.samplerAnisotropy) {
			score += 4;
			log().debug(log().tab(), "Supports anistropic filtering");
		}
	}
	log().info("Score: ", score, log().end_l());
	map.insert(std::make_pair(score, device));
}

void Device::create_queue(QueueFamily& queue) noexcept {
	queue.queue = vk::Queue(m_device, queue.familyIndex, 0);
}

void Device::create_instance() {
	// check if requested validation layers are available
#ifndef NDEBUG
	uint32 availableLayerCount = 0;
	vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
	std::vector <VkLayerProperties> availableLayers(availableLayerCount);
	vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

	check_requested_validation_layers(availableLayers, settings().debug.requestedValidationLayers);
#endif
	// get all extensions
	uint32 SDLExtensionCount = 0;

	lassert(SDL_Vulkan_GetInstanceExtensions(Application::window.get(), &SDLExtensionCount, nullptr) == SDL_TRUE, "Failed to get number of Vulkan instance extensions");
	std::vector<const char*> SDLExtensions(SDLExtensionCount);
	lassert(SDL_Vulkan_GetInstanceExtensions(Application::window.get(), &SDLExtensionCount, SDLExtensions.data()) == SDL_TRUE, "Failed to get Vulkan instance extensions");
	SDLExtensions.push_back("VK_KHR_get_physical_device_properties2");
#ifdef __APPLE__
	SDLExtensions.push_back("VK_KHR_portability_enumeration");
#endif

	// define some info for the application that will be used in instance creation
	VkApplicationInfo appInfo{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		settings().window.title.c_str(),
		VK_MAKE_VERSION(0, 0, 1),
		"LyraEngine",
		VK_MAKE_VERSION(0, 7, 0),
		VK_API_VERSION_1_3
	};

	// defining some instance info
	VkInstanceCreateInfo createInfo{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
#ifdef _WIN32
		0,
#elif __APPLE__
		VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
#endif
		&appInfo,
#ifndef NDEBUG
		static_cast<uint32>(settings().debug.requestedValidationLayers.size()),
		settings().debug.requestedValidationLayers.data(),
#else
		0,
		nullptr,
#endif
		static_cast<uint32>(SDLExtensions.size()),
		SDLExtensions.data()
	};

	// create the instance
	m_instance = vk::Instance(VK_NULL_HANDLE, createInfo);
}

void Device::pick_physical_device() {
	// get all devices
	uint32 deviceCount = 0;
	vassert(vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr), "find any Vulkan auitable GPUs");
		std::vector <VkPhysicalDevice> devices(deviceCount);			 // just put this in here cuz I was lazy
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

	// a ordered map with every GPU. The one with the highest score is the one that is going to be the used GPU
	std::multimap <int, VkPhysicalDevice> possibleDevices;

	for (const auto& device : devices) {
		log().info("GPU " + std::to_string((&device - &devices[0]) + 1) + ": ");
		rate_physical_device(device, possibleDevices);
	}

	if (possibleDevices.begin()->first <= 0) {
		log().exception("Failed to find GPU with enough features");
	}

	m_physicalDevice = std::move(vk::PhysicalDevice(possibleDevices.begin()->second, m_instance));
}

void Device::create_logical_device() {
	std::vector <VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32> queueFamilies = { m_graphicsQueue.familyIndex, m_presentQueue.familyIndex };

	float queuePriority = 1.0f;
	for (const auto& familyIndex : queueFamilies) {
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
#ifndef NDEBUG
		static_cast<uint32>(settings().debug.requestedValidationLayers.size()),
		settings().debug.requestedValidationLayers.data(),
#else
		0,
		nullptr,
#endif
		static_cast<uint32>(settings().debug.requestedDeviceExtensions.size()),
		settings().debug.requestedDeviceExtensions.data(),
		&deviceFeatures
	};

	// create the device and retrieve the graphics and presentation queue handles
	m_device = vk::Device(m_physicalDevice, createInfo);

	find_family_index(m_graphicsQueue, m_physicalDevice);
	find_family_index(m_presentQueue, m_physicalDevice);
	create_queue(m_graphicsQueue);
	create_queue(m_presentQueue);
}

void Device::create_allocator() {
	// creation info
	VmaAllocatorCreateInfo createInfo {
		0,
		m_physicalDevice,
		m_device,
		0,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		m_instance,
		VK_API_VERSION_1_2
	};

	// create the allocator
	m_allocator = vma::Allocator(m_instance, createInfo);
}

} // namespace vulkan

} // namespace lyra