#include <Graphics/VulkanImpl/Device.h>

#include <Common/Array.h>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <Common/Settings.h>

#include <Graphics/SDLWindow.h>
#include <Application/Application.h>

namespace lyra {

namespace vulkan {

Device::Device() {
	{ // create instance
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
	
	{ // find a suitable physical device
		// get all devices
		uint32 deviceCount = 0;
		vassert(vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr), "find any Vulkan suitable GPUs");
		std::vector <VkPhysicalDevice> devices(deviceCount);			 // just put this in here cuz I was lazy
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

		// a ordered map with every GPU. The one with the highest score is the one that is going to be the used GPU
		std::multimap <uint32, std::pair<VkPhysicalDevice, QueueFamilies>> possibleDevices;

		// create a temprary surface to check present queue compatibility
		vk::Surface surface(m_instance, Application::window.get());

		for (const auto& device : devices) {
			log().info("GPU ", std::to_string((&device - &devices[0]) + 1), ": ");
			rate_physical_device(device, surface, possibleDevices);
		}

		if (possibleDevices.rbegin()->first <= 0) {
			log().exception("Failed to find GPU with enough features");
		}

		m_physicalDevice = std::move(vk::PhysicalDevice(possibleDevices.rbegin()->second.first, m_instance));
		m_queueFamilies = possibleDevices.rbegin()->second.second;
	}

	{ // create logical device
		std::vector <VkDeviceQueueCreateInfo> queueCreateInfos;
		Array<uint32, 2> queueFamilies = { m_queueFamilies.graphicsComputeQueueIndex, m_queueFamilies.presentQueueIndex };

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
	}

	{ // create queues
		m_graphicsQueue = vk::Queue(m_device, m_queueFamilies.graphicsComputeQueueIndex, 0);
		m_presentQueue = vk::Queue(m_device, m_queueFamilies.presentQueueIndex, 0);
		m_computeQueue  = vk::Queue(m_device, m_queueFamilies.graphicsComputeQueueIndex, 0);
	}
	
	{ // create the memory allocator
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
}

bool Device::check_requested_extensions(const std::vector <VkExtensionProperties>& extensions, const std::vector <const char*>& requestedExtensions) const {
#ifndef NDEBUG
	// print all all availabe extensions
	log().info("Available device extensions:");
	for (const auto& extension : extensions) log().debug(log().tab(), extension.extensionName);
#endif
	// go through every requested extensions and see if they are available
	for (const auto& requestedExtension : requestedExtensions) {
		for (const auto& extension : extensions) {
			if (strcmp(requestedExtension, extension.extensionName) == 0) {
				break;
			} else {
				return false;
			}
		}
	}

	return true;
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

Device::QueueFamilies Device::find_queue_families(const VkPhysicalDevice& device, const vk::Surface& surface) noexcept {
	// queue family structure
	QueueFamilies queueFamilies;

	// get the properties and queue count
	uint32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> properties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, properties.data());

	uint32 i = 0;
	for (const auto& property : properties) {
		if ((property.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (property.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
			queueFamilies.graphicsComputeQueueIndex = i;
		}

		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			queueFamilies.presentQueueIndex = i;
		}

		if ((queueFamilies.found())) {
			break;
		}

		i++;
	}

	return queueFamilies;
}

void Device::rate_physical_device(const VkPhysicalDevice& device, const vk::Surface& surface, std::multimap <uint32, std::pair<VkPhysicalDevice, QueueFamilies>>& map) {
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

	// get the queue families
	QueueFamilies localQueueFamilies = find_queue_families(device, surface);

	// set the score to 1 first, if the GPU does not have required features, set it to 0
	int score = 1;

	// some required features. If not available, make the GPU unavailable
	if (
		!features.multiDrawIndirect || 
		!localQueueFamilies.found() || 
		check_requested_extensions(availableExtensions, settings().debug.requestedDeviceExtensions)
	) {
		score = 0;
		log().warning("GPU does not have some required features!");
	}

	log().info("Available device features and properties: ");

	// the actual scoring system
	if (score > 0) {
		score = 0;
		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) { // cpu type
			score += 20;
			log().debug(log().tab(), "Discrete GPU");
		} if (features.samplerAnisotropy) {
			score += 4;
			log().debug(log().tab(), "Supports anistropic filtering");
		} 
		score += (int)(properties.limits.maxImageDimension2D / 2000);

		log().info("Score: ", score, log().end_l());
	}

	// insert the device into the queue
	map.emplace(score, std::make_pair(device, localQueueFamilies));
}

} // namespace vulkan

} // namespace lyra
