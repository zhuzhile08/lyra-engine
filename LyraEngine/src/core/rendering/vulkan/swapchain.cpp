#include <core/rendering/vulkan/swapchain.h>

namespace lyra {

// swapchain images
void VulkanSwapchain::VulkanSwapchainImages::create(const VulkanDevice* const device, const VulkanSwapchain* const swapchain) {
	Logger::log_info("Creating Vulkan swapchain images...");

	this->device = device;

	// get the number of images
	uint32 imageCount;
	lassert(vkGetSwapchainImagesKHR(device->device(), swapchain->swapchain(), &imageCount, nullptr) == VK_SUCCESS, "Failed to retrieve Vulkan swapchain images!");
	_images.resize(imageCount); _views.resize(imageCount);
	vkGetSwapchainImagesKHR(device->device(), swapchain->swapchain(), &imageCount, _images.data());

	// I hate this bro why C++
	// this code stems from the disability to have a vector with my own image type, because then vkGetSwapchainImagesKHR won't work properly, so I had to separate everything again
	for (uint32 i = 0; i < imageCount; i++) {
		// image view creation info
		VkImageViewCreateInfo createInfo{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			nullptr,
			0,
			_images.at(i),
			VK_IMAGE_VIEW_TYPE_2D,
			swapchain->format(),
			{ VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY },
			{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
		};

		// create the view
		lassert(vkCreateImageView(device->device(), &createInfo, nullptr, &_views.at(i)) == VK_SUCCESS, "Failed to create Vulkan image views");
	}

	Logger::log_info("Successfully created Vulkan swapchain images at ", get_address(this), "!", Logger::end_l());
}

// color resources
void VulkanSwapchain::VulkanColorResources::create(const VulkanDevice* const device, const VulkanSwapchain* const swapchain) {
	Logger::log_info("Creating Vulkan color resources...");

	this->device = device;

	VkFormat colorFormat = swapchain->format();

	_maxSamples = getMaxSamples();

	// create memory and image
	lassert(vmaCreateImage(device->allocator(),
		&get_image_create_info(
			colorFormat,
			{ swapchain->extent().width, swapchain->extent().height, 1 },
			VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			1,
			VK_IMAGE_TYPE_2D,
			1,
			_maxSamples
		),
		&get_alloc_create_info(device, VMA_MEMORY_USAGE_GPU_ONLY, VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)),
		&_image,
		&_memory,
		nullptr
	) == VK_SUCCESS, "Failed to create Vulkan color resources!");

	// create the image view
	create_view(device, colorFormat, { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

	Logger::log_info("Successfully created Vulkan color resources at ", get_address(this), "!", Logger::end_l());
}

// depth buffer
void VulkanSwapchain::VulkanDepthBuffer::create(const VulkanDevice* const device, const VulkanSwapchain* const swapchain, const VulkanColorResources* const multisampling, CommandBufferManager* const commandBufferManager) {
	Logger::log_info("Creating Vulkan depth buffer...");

	this->device = device;

	_format = get_best_format(device, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_TILING_OPTIMAL);

	// create memory and image
	lassert(vmaCreateImage(device->allocator(),
		&get_image_create_info(
			_format,
			{ swapchain->extent().width, swapchain->extent().height, 1 },
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			1,
			VK_IMAGE_TYPE_2D,
			1,
			multisampling->maxSamples()
		),
		&get_alloc_create_info(Application::context()->device(), VMA_MEMORY_USAGE_GPU_ONLY, VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)),
		&_image,
		&_memory,
		nullptr
	) == VK_SUCCESS, "Failed to create Vulkan depth buffer!");

	// create the image view
	create_view(VK_FORMAT_D32_SFLOAT, { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 });

	// transition the image layout
	transition_layout(commandBufferManager, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, _format, { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 });

	Logger::log_info("Successfully created Vulkan depth buffer at ", get_address(this), "!", Logger::end_l());
}

// swap chain
void VulkanSwapchain::create(const VulkanDevice* const device, const VulkanInstance* const instance, CommandBufferManager* const commandBufferManager, const Window* const window) {
	Logger::log_info("Creating Vulkan swapchain...");

	this->device = device;
	this->instance = instance;
	this->window = window;
	this->commandBufferManager = commandBufferManager;
	create_swapchain(commandBufferManager);

	Logger::log_info("Successfully created Vulkan swapchain at ", get_address(this), "!", Logger::end_l());
}

void VulkanSwapchain::recreate() {
	// wait until all commands are done executing
	vkDeviceWaitIdle(device->device());

	// destroy the images and depth buffer
	destroy();

	// recreate the swapchain
	create_swapchain(commandBufferManager);

	Logger::log_info("Successfully recreated Vulkan swapchain at ", get_address(this), "!", Logger::end_l());
}

void VulkanSwapchain::create_swapchain_extent(const VkSurfaceCapabilitiesKHR surfaceCapabilities) {
	int width, height;
	SDL_Vulkan_GetDrawableSize(window->get(), &width, &height);

	VkExtent2D newExtent = {
		static_cast<uint32>(width),
		static_cast<uint32>(height)
	};

	newExtent.width = std::clamp(newExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
	newExtent.height = std::clamp(newExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

	_extent = newExtent;

	Logger::log_debug(Logger::tab(), "width is ", width, " and the height is ", height);
}

const VkSurfaceFormatKHR VulkanSwapchain::get_optimal_format() {
	uint32 availableFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device->physicalDevice(), instance->surface(), &availableFormatCount, nullptr);
	std::vector <VkSurfaceFormatKHR> availableFormats(availableFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device->physicalDevice(), instance->surface(), &availableFormatCount, availableFormats.data());
	// check the formats
	for (uint32 i = 0; i < availableFormats.size(); i++) {
		if (availableFormats.at(i).format == VK_FORMAT_B8G8R8A8_SRGB && availableFormats.at(i).colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			_format = availableFormats.at(i).format;
			return availableFormats.at(i);
		}
	}

	_format = availableFormats.at(0).format;
	return availableFormats.at(0);
}

const VkPresentModeKHR VulkanSwapchain::get_optimal_present_mode() {
	uint32 availablePresentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device->physicalDevice(), instance->surface(), &availablePresentModeCount, nullptr);
	std::vector <VkPresentModeKHR> availablePresentModes(availablePresentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device->physicalDevice(), instance->surface(), &availablePresentModeCount, availablePresentModes.data());
	// check the presentation modess
	for (uint32 i = 0; i < availablePresentModes.size(); i++) {
		if (availablePresentModes.at(i) == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentModes.at(i);
			break;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

void VulkanSwapchain::check_surface_capabilities(VkSurfaceCapabilitiesKHR& surfaceCapabilities) const {
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->physicalDevice(), instance->surface(), &surfaceCapabilities);

	if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) {
		surfaceCapabilities.currentExtent.width = Settings::Window::width;
		Logger::log_warning("Something went wrong whilst attempting getting the swapchain width!");
	} if (surfaceCapabilities.currentExtent.height == 0xFFFFFFFF) {
		surfaceCapabilities.currentExtent.height = Settings::Window::height;
		Logger::log_warning("Something went wrong whilst attempting getting the swapchain height!");
	} if (surfaceCapabilities.maxImageCount == 0xFFFFFFFF) {
		surfaceCapabilities.maxImageCount = 8;
		Logger::log_warning("Something went wrong whilst attempting getting the number of swapchain images!");
	} if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
		surfaceCapabilities.supportedUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	} if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		surfaceCapabilities.supportedTransforms = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
}

const VkSampleCountFlagBits VulkanSwapchain::VulkanColorResources::getMaxSamples() const noexcept {
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(device->physicalDevice(), &physicalDeviceProperties);

	VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
	if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
	if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
	if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

	return VK_SAMPLE_COUNT_1_BIT;
}

void VulkanSwapchain::create_swapchain(CommandBufferManager* const commandBufferManager) {
	Logger::log_debug(Logger::tab(), "Swapchain configurations are: ");

	// get the optimal format
	VkSurfaceFormatKHR format = get_optimal_format();
	Logger::log_debug(Logger::tab(), "format is ", _format, " (preferred format is format ", VK_FORMAT_B8G8R8A8_SRGB, " with color space ", VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, ")");
	
	VkPresentModeKHR presentMode = get_optimal_present_mode();
	Logger::log_debug(Logger::tab(), "present mode is ", presentMode, " (preferred present mode is mode ", VK_PRESENT_MODE_MAILBOX_KHR, ")");

	// set the surface capabilities if something went wrong
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	check_surface_capabilities(surfaceCapabilities);

	// create the extent
	create_swapchain_extent(surfaceCapabilities);

	// create the swapchain
	const auto temp = device->graphicsQueue().familyIndex;
	const auto cond = (device->graphicsQueue().familyIndex != device->presentQueue().familyIndex);

	VkSwapchainCreateInfoKHR createInfo = {
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		0,
		instance->surface(),
		surfaceCapabilities.minImageCount + 1,
		format.format,
		format.colorSpace,
		_extent,
		1,
		surfaceCapabilities.supportedUsageFlags,
		(cond) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
		static_cast<uint32>((cond) ? 2 : 0),
		(cond) ? &temp : 0,
		surfaceCapabilities.currentTransform,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		presentMode,
		VK_TRUE,
		(_oldSwapchain != nullptr) ? *_oldSwapchain : VK_NULL_HANDLE
	};

	lassert(vkCreateSwapchainKHR(device->device(), &createInfo, nullptr, &_swapchain) == VK_SUCCESS, "Failed to create Vulkan swapchain");

	_images.create(device, this);
	_colorResources.create(device, this);
	_depthBuffer.create(device, this, &_colorResources, commandBufferManager);
}

} // namespace lyra