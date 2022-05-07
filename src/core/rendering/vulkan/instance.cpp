#include <core/rendering/vulkan/instance.h> 

namespace lyra {

VulkanInstance::VulkanInstance() { }

VulkanInstance::~VulkanInstance() noexcept {
	vkDestroySurfaceKHR(_instance, _surface, nullptr);
	vkDestroyInstance(_instance, nullptr);

	Logger::log_info("Succesfully destroyed Vulkan instance!");
}

void VulkanInstance::destroy() noexcept {
	this->~VulkanInstance();
}

void VulkanInstance::create(const Window* const window) {
	Logger::log_info("Creating Vulkan instance...");

	this->window = window;

	create_instance();
	create_window_surface();

	Logger::log_info("Succesfully created Vulkan instance at ", get_address(this), "!", Logger::end_l());
}

void VulkanInstance::check_requested_validation_layers(const std::vector <VkLayerProperties> layers, const std::vector <const char*> requestedLayers) const {
	// go through every requested layers and see if they are available
	for (const char* layer : requestedLayers) {
		bool found = false;
		Logger::log_info("Available layers:");

		for (const auto& layerProperties : layers) {
			Logger::log_debug(Logger::tab(), layerProperties.layerName, layerProperties.description);
			if (strcmp(layer, layerProperties.layerName) == 0) {
				found = true;
				break;
			}
		}

		if (!found) {
			Logger::log_exception("User required Vulkan validation layer wasn't found: ", layer);
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

	check_requested_validation_layers(availableLayers, Settings::Debug::requestedValidationLayers);
#endif
	// get all extensions
	uint32 SDLExtensionCount = 0;

	if(!SDL_Vulkan_GetInstanceExtensions(window->get(), &SDLExtensionCount, nullptr)) Logger::log_exception("Failed to get number of Vulkan instance extensions");
	const char** SDLExtensions = new const char* [SDLExtensionCount];
	if(!SDL_Vulkan_GetInstanceExtensions(window->get(), &SDLExtensionCount, SDLExtensions)) Logger::log_exception("Failed to get Vulkan instance extensions");

	// define some info for the application that will be used in instance creation
	VkApplicationInfo appInfo {
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		Settings::Window::title,
		VK_MAKE_VERSION(0, 0, 1),
		Settings::Window::title,
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
		static_cast<uint32>(Settings::Debug::requestedValidationLayers.size()),
		Settings::Debug::requestedValidationLayers.data(),
#else
		0,
		nullptr
#endif
		SDLExtensionCount,
		SDLExtensions
	};


// create the instance
	if(vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) Logger::log_exception("Failed to create Vulkan instance");
}

void VulkanInstance::create_window_surface() {
	// thankfully, SDL can handle the platform specific stuff for creating surfaces for me, which makes it all way easier
	if(!SDL_Vulkan_CreateSurface(window->get(), _instance, &_surface)) Logger::log_exception("Failed to create Vulkan window surface");
}

} // namespace lyra