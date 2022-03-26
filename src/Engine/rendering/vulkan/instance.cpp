#include <rendering/vulkan/instance.h> 

namespace lyra {

VulkanInstance::VulkanInstance() { }

void VulkanInstance::destroy() noexcept {
	vkDestroySurfaceKHR(_instance, _surface, nullptr);
    vkDestroyInstance(_instance, nullptr);

	delete window;

	LOG_INFO("Succesfully destroyed Vulkan instance!")
}

void VulkanInstance::create(Window window) {
	LOG_INFO("Creating Vulkan instance...")

	this->window = new Window(window);

	create_instance();
	create_window_surface();

	LOG_INFO("Succesfully created Vulkan instance at ", GET_ADDRESS(this), "!", END_L)
}

void VulkanInstance::check_requested_validation_layers(const std::vector <VkLayerProperties> layers, const std::vector <str> requestedLayers) const {
	// go through every requested layers and see if they are available
	for (str layer : requestedLayers) {
		bool found = false;
		LOG_INFO("Available layers:")

		for (const auto& layerProperties : layers) {
			LOG_DEBUG(TAB, layerProperties.layerName, layerProperties.description)
			if (strcmp(layer, layerProperties.layerName) == 0) {
				found = true;
				break;
			}
		}

		if (!found) {
			LOG_EXEPTION("User required Vulkan validation layer wasn't found: ", layer)
		}
	}
}

void VulkanInstance::create_instance() {
	// check if requested validation layers are available
#ifndef ndebug
	uint32 availableLayerCount = 0;
	vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
	std::vector <VkLayerProperties> availableLayers(availableLayerCount);
	vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

	check_requested_validation_layers(availableLayers, requested_validation_layers);
#endif
	// get all extensions
	uint32 SDLExtensionCount = 0;

	if(!SDL_Vulkan_GetInstanceExtensions(window->get_window(), &SDLExtensionCount, nullptr)) LOG_EXEPTION("Failed to get number of Vulkan instance extensions")
	str* SDLExtensions = new str [SDLExtensionCount];
	if(!SDL_Vulkan_GetInstanceExtensions(window->get_window(), &SDLExtensionCount, SDLExtensions)) LOG_EXEPTION("Failed to get Vulkan instance extensions")

	// define some info for the application that will be used in instance creation
	VkApplicationInfo appInfo {
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		TITLE,
		VK_MAKE_VERSION(0, 0, 1),
		TITLE,
		VK_MAKE_VERSION(0, 0, 1),
		VK_API_VERSION_1_2
	};

	// defining some instance info
	VkInstanceCreateInfo createInfo {
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0,
		&appInfo,
#ifndef ndebug
		static_cast<uint32>(requested_validation_layers.size()),
		requested_validation_layers.data(),
#else
		0,
		nullptr
#endif
		SDLExtensionCount,
		SDLExtensions
	};


// create the instance
	if(vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) LOG_EXEPTION("Failed to create Vulkan instance")
}

void VulkanInstance::create_window_surface() {
	// thankfully, SDL can handle the platform specific stuff for creating surfaces for me, which makes it all way easier
	if(!SDL_Vulkan_CreateSurface(window->get_window(), _instance, &_surface)) LOG_EXEPTION("Failed to create Vulkan window surface")
}

} // namespace lyra