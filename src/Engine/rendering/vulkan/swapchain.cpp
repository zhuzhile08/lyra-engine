#include <rendering/vulkan/swapchain.h>

namespace lyra {
	
// swap chain images
VulkanSwapchain::VulkanSwapchainImages::VulkanSwapchainImages() { }

void VulkanSwapchain::VulkanSwapchainImages::destroy() {
	for (auto& image : images) {
		image.destroy();
		vkDestroyImage(device->get().device, image.image, nullptr);
	}

	delete device;

	LOG_INFO("Succesfully destroyed Vulkan swapchain images!")
}

void VulkanSwapchain::VulkanSwapchainImages::create(VulkanDevice device, const VulkanSwapchain swapchain) {
	LOG_INFO("Creating Vulkan swapchain images...")

	this->device = &device;

	// get the number of images
	uint32 						imageCount;
	vkGetSwapchainImagesKHR(device.get().device, swapchain.get().swapchain, &imageCount, nullptr);
	images.resize(imageCount);

	for (size_t i = 0; i < imageCount; i++) {
		vkGetSwapchainImagesKHR(device.get().device, swapchain.get().swapchain, &imageCount, &images[i].image);
		images[i].create_view(device, swapchain.get().format, {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
	}

	LOG_INFO("Succesfully created Vulkan swapchain images at ", GET_ADDRESS(this), "!", END_L)
}

// depth buffer
VulkanSwapchain::VulkanDepthBuffer::VulkanDepthBuffer() { }

void VulkanSwapchain::VulkanDepthBuffer::destroy() {
	image.destroy();
	vmaDestroyImage(device->get().allocator, image.image, memory);

	delete device;

	LOG_INFO("Succesfully destroyed depth buffer!")
}

void VulkanSwapchain::VulkanDepthBuffer::create(VulkanDevice device, const VulkanSwapchain swapchain) {
	LOG_INFO("Creating Vulkan depth buffer...")

	this->device = &device;

	// memory allocation info
	VmaAllocationCreateInfo allocCreateInfo = {
		0,
		VMA_MEMORY_USAGE_GPU_ONLY,
		VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	};

	// create memory and image
	vmaCreateImage(device.get().allocator, &image.get_image_create_info(
			device, VK_FORMAT_D32_SFLOAT, {swapchain.get().extent.width, swapchain.get().extent.height, 1}, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
		), &allocCreateInfo, &image.image, &memory, nullptr
	);

	// create the image view
	image.create_view(device, VK_FORMAT_D32_SFLOAT, {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1});

	LOG_INFO("Succesfully created Vulkan depth buffer at ", GET_ADDRESS(this), "!", END_L)
}

// swap chain
VulkanSwapchain::VulkanSwapchain() { }

void VulkanSwapchain::destroy() {
	var.images.destroy();
	var.depthBuffer.destroy();

	vkDestroySwapchainKHR(device->get().device, var.swapchain, nullptr);

	delete var.oldSwapchain;
	delete device;
	delete instance;
	delete window;

	LOG_INFO("Succesfully destroyed Vulkan swapchain!")
}

void VulkanSwapchain::create(VulkanDevice device, VulkanInstance instance, Window window) {
	LOG_INFO("Creating Vulkan swapchain...")

	this->device = &device;
	this->instance = &instance;
	this->window = &window;
	create_swapchain();

	LOG_INFO("Succesfully created Vulkan swapchain at ", GET_ADDRESS(this), "!", END_L)
}

void VulkanSwapchain::create(VulkanDevice device, VulkanInstance instance, Window window, VulkanSwapchain oldSwapchain) {
	var.oldSwapchain = &oldSwapchain;
	create(device, instance, window);

	LOG_INFO("Succesfully recreated Vulkan swapchain at ", GET_ADDRESS(this), "!", END_L)
}

void VulkanSwapchain::create_swapchain_extent(VkSurfaceCapabilitiesKHR *surfaceCapabilities) {
	if (surfaceCapabilities->currentExtent.width == UINT32_MAX) {           // if something is wrong, fix the extent
		LOG_WARNING("Couldn't get Vulkan swapchain capabilities' width")
		int width, height;
		SDL_GL_GetDrawableSize(window->get_window(), &width, &height);

		VkExtent2D newExtent = {
			static_cast<uint32>(width),
			static_cast<uint32>(height)
		};

		auto clamp = [ ] (int a, int min, int max) {
			if (a <= min) {
				return min;
			} else {
				if (a >= max) {
					return max;
				} else {
					return a;
				}
			}
		};

		newExtent.width = clamp(newExtent.width, surfaceCapabilities->minImageExtent.width, surfaceCapabilities->maxImageExtent.width);
		newExtent.height = clamp(newExtent.height, surfaceCapabilities->minImageExtent.height, surfaceCapabilities->maxImageExtent.height);

		var.extent = newExtent;

	} else {                                                                // in this case, the extent is fine
		var.extent = surfaceCapabilities->currentExtent;
	}
}

void VulkanSwapchain::create_swapchain() {
	// query some details
	uint32                              availableFormatCount = 0;    	   // formats
	VkSurfaceFormatKHR                  format;
	uint32                              availablePresentModeCount = 0;      // presentation modes
	VkPresentModeKHR                    presentMode;

	if(vkGetPhysicalDeviceSurfaceFormatsKHR(device->get().physicalDevice, instance->get().surface, &availableFormatCount, nullptr) != VK_SUCCESS)
		LOG_EXEPTION("Failed to get available swapchain surface modes")
	if(vkGetPhysicalDeviceSurfacePresentModesKHR(device->get().physicalDevice, instance->get().surface, &availablePresentModeCount, nullptr) != VK_SUCCESS)
		LOG_EXEPTION("Failed to get available swapchain present modes")

	// check the formats
	std::vector <VkSurfaceFormatKHR>    availableFormats(availableFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device->get().physicalDevice, instance->get().surface, &availableFormatCount, availableFormats.data());
	for (auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			format = availableFormat;
			break;
		} else {
			format = availableFormats[0];
		}
	}

	LOG_INFO("Swapchain configurations are: ")

	var.format = format.format;

	LOG_DEBUG(TAB, "format is ", var.format, " (prefered format is format ", VK_FORMAT_B8G8R8A8_SRGB, " with color space ", VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, ")")

	// check the presentation modes
	std::vector <VkPresentModeKHR>      availablePresentModes(availablePresentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device->get().physicalDevice, instance->get().surface, &availablePresentModeCount, availablePresentModes.data());
	for (const auto& mode : availablePresentModes) {
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			presentMode = mode;
			break;
		} else {
			presentMode = VK_PRESENT_MODE_FIFO_KHR;
		}
	}

	LOG_DEBUG(TAB, "present mode is ", presentMode, " (prefered present mode is mode ", VK_PRESENT_MODE_MAILBOX_KHR, ")")

	// set the surface capabilities if something went wrong
	VkSurfaceCapabilitiesKHR            surfaceCapabilities;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->get().physicalDevice, instance->get().surface, &surfaceCapabilities);

	if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) {
		surfaceCapabilities.currentExtent.width = WIDTH;
		LOG_WARNING("Something went wrong whilst attempting getting the swapchain width!")
	} if (surfaceCapabilities.currentExtent.height == 0xFFFFFFFF) {
		surfaceCapabilities.currentExtent.height = HEIGHT;
		LOG_WARNING("Something went wrong whilst attempting getting the swapchain height!")
	} if (surfaceCapabilities.maxImageCount == 0xFFFFFFFF) {
		surfaceCapabilities.maxImageCount = 8;
		LOG_WARNING("Something went wrong whilst attempting getting the number of swapchain images!")
	} if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT ) {
		surfaceCapabilities.supportedUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	} if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ) {
		surfaceCapabilities.supportedTransforms = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	} 

	LOG_DEBUG(TAB, "width is ", WIDTH, " and the height is ", HEIGHT)

	// create the extent
	create_swapchain_extent(&surfaceCapabilities);

	// create the swapchain
	auto temp = device->get().graphicsQueue.familyIndex;

	VkSwapchainCreateInfoKHR            createInfo;
	if (device->get().graphicsQueue.familyIndex != device->get().presentQueue.familyIndex) {
		createInfo = {
			VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			nullptr,
			0,
			instance->get().surface,
			surfaceCapabilities.minImageCount + 1,
			format.format,
			format.colorSpace,
			var.extent,
			1,
			surfaceCapabilities.supportedUsageFlags,
			VK_SHARING_MODE_CONCURRENT,
			2,
			&temp,
			surfaceCapabilities.currentTransform,
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			presentMode,
			VK_TRUE,
			(var.oldSwapchain != nullptr) ? var.oldSwapchain->var.swapchain : VK_NULL_HANDLE
		};
	} else {
		createInfo = {
			VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			nullptr,
			0,
			instance->get().surface,
			surfaceCapabilities.minImageCount,
			format.format,
			format.colorSpace,
			var.extent,
			1,
			surfaceCapabilities.supportedUsageFlags,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			0,
			surfaceCapabilities.currentTransform,
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			presentMode,
			true,
			(var.oldSwapchain != nullptr) ? var.oldSwapchain->var.swapchain : VK_NULL_HANDLE
		};
	}

	if(vkCreateSwapchainKHR(device->get().device, &createInfo, nullptr, &var.swapchain) != VK_SUCCESS) LOG_EXEPTION("Failed to create Vulkan swapchain")

	var.images.create(*device, *this);
	var.depthBuffer.create(*device, *this);
}

VulkanSwapchain::Variables VulkanSwapchain::get() const {
	return var;
}

} // namespace lyra
