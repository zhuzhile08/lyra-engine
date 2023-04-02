#include <Graphics/VulkanImpl/Window.h>

#include <algorithm>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <Common/Settings.h>

#include <Graphics/VulkanImpl/Device.h>
#include <Graphics/VulkanImpl/CommandBuffer.h>

#include <Application/Application.h>

namespace lyra {

namespace vulkan {

Window::Window() : m_surface(Application::renderSystem.device.instance(), Application::window.get()) {
	create_swapchain();
}

void Window::recreate() {
	// destroy the images and views
	for (auto& imageView : m_imageViews)
		imageView.destroy();
	m_depthImage.destroy();
	m_colorImage.destroy();
	m_depthMem.destroy();
	m_colorMem.destroy();

	// destroy the old swapchain
	if (m_oldSwapchain != nullptr) vkDestroySwapchainKHR(Application::renderSystem.device.device(), *m_oldSwapchain, nullptr);

	// reassign the old swapchain
	m_oldSwapchain = std::move(&this->m_swapchain);

	// recreate the swapchain
	create_swapchain();
}

void Window::create_swapchain_extent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) {
	int width, height;
	SDL_Vulkan_GetDrawableSize(Application::window.get(), &width, &height);

	VkExtent2D newExtent = {
		static_cast<uint32>(width),
		static_cast<uint32>(height)
	};

	newExtent.width = std::clamp(newExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
	newExtent.height = std::clamp(newExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

	m_extent = newExtent;
}

const VkSurfaceFormatKHR Window::get_optimal_format() {
	uint32 availableFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(Application::renderSystem.device.physicalDevice(), m_surface, &availableFormatCount, nullptr);
	std::vector <VkSurfaceFormatKHR> availableFormats(availableFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(Application::renderSystem.device.physicalDevice(), m_surface, &availableFormatCount, availableFormats.data());
	// check the formats
	for (const auto& format : availableFormats) {
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			m_format = format.format;
			return format;
		}
	}

	m_format = availableFormats[0].format;
	return availableFormats[0];
}

const VkPresentModeKHR Window::get_optimal_present_mode() const {
	uint32 availablePresentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(Application::renderSystem.device.physicalDevice(), m_surface, &availablePresentModeCount, nullptr);
	std::vector <VkPresentModeKHR> availablePresentModes(availablePresentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(Application::renderSystem.device.physicalDevice(), m_surface, &availablePresentModeCount, availablePresentModes.data());
	// check the presentation modess
	for (const auto& presentMode : availablePresentModes) {
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return presentMode;
			break;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

const VkSampleCountFlagBits Window::get_max_samples() const noexcept {
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(Application::renderSystem.device.physicalDevice(), &physicalDeviceProperties);

	VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
	if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
	if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
	if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

	return VK_SAMPLE_COUNT_1_BIT;
}

void Window::check_surface_capabilities(VkSurfaceCapabilitiesKHR& surfaceCapabilities) const {
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Application::renderSystem.device.physicalDevice(), m_surface, &surfaceCapabilities);

	if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) {
		surfaceCapabilities.currentExtent.width = settings().window.width;
		log().warning("Something went wrong whilst attempting getting the swapchain width!");
	} if (surfaceCapabilities.currentExtent.height == 0xFFFFFFFF) {
		surfaceCapabilities.currentExtent.height = settings().window.height;
		log().warning("Something went wrong whilst attempting getting the swapchain height!");
	} if (surfaceCapabilities.maxImageCount == 0xFFFFFFFF) {
		surfaceCapabilities.maxImageCount = 8;
		log().warning("Something went wrong whilst attempting getting the number of swapchain images!");
	} if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
		surfaceCapabilities.supportedUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	} if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		surfaceCapabilities.supportedTransforms = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
}

void Window::create_swapchain_images() {
	std::vector<VkImage> tempImages;
	// get the number of images
	uint32 imageCount;
	vassert(vkGetSwapchainImagesKHR(Application::renderSystem.device.device(), m_swapchain, &imageCount, nullptr), "retrieve Vulkan swapchain images");
	m_images.resize(imageCount); m_imageViews.resize(imageCount); tempImages.resize(imageCount);
	vkGetSwapchainImagesKHR(Application::renderSystem.device.device(), m_swapchain, &imageCount, tempImages.data());

	// create the image views
	for (auto& image : m_images) {
		uint32 i = &image - &m_images[0];
		image = std::move(tempImages[i]);

		// image view creation info
		VkImageViewCreateInfo createInfo{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			nullptr,
			0,
			image,
			VK_IMAGE_VIEW_TYPE_2D,
			m_format,
			{ VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY },
			{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
		};

		// create the view
		m_imageViews[i] = vk::ImageView(Application::renderSystem.device.device(), createInfo);
	}
}

void Window::create_depth_buffer() {
	m_depthBufferFormat = Image::get_best_format({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_TILING_OPTIMAL);

	// create memory and image
	m_depthImage.m_image = vk::Image(
		Application::renderSystem.device.device(),
		Application::renderSystem.device.allocator(), 
		m_depthImage.get_image_create_info(
			m_depthBufferFormat,
			{ m_extent.width, m_extent.height, 1 },
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			1,
			VK_IMAGE_TYPE_2D,
			1,
			0,
			m_maxMultisamples
		),
		GPUMemory::get_alloc_create_info(VMA_MEMORY_USAGE_GPU_ONLY, VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)),
		m_depthMem.memory()
	);

	// create the image view
	m_depthImage.create_view(VK_FORMAT_D32_SFLOAT, { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 });
}

void Window::create_color_resources() {
	m_maxMultisamples = get_max_samples();

	m_colorImage.m_image = vk::Image(
		Application::renderSystem.device.device(),
		Application::renderSystem.device.allocator(), 
		m_colorImage.get_image_create_info(
			m_format,
			{ m_extent.width, m_extent.height, 1 },
			VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			1,
			VK_IMAGE_TYPE_2D,
			1,
			0,
			m_maxMultisamples
		),
		GPUMemory::get_alloc_create_info(VMA_MEMORY_USAGE_GPU_ONLY, VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)),
		m_colorMem.memory()
	);

	// create the image view
	m_colorImage.create_view(m_format, { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
}

void Window::create_swapchain() {
	log().info("Swapchain configurations are: ");

	// get the optimal format
	VkSurfaceFormatKHR format = get_optimal_format();
	log().debug(log().tab(), "format is ", m_format, " (preferred format is format ", VK_FORMAT_B8G8R8A8_SRGB, " with color space ", VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, ")");
	
	VkPresentModeKHR presentMode = get_optimal_present_mode();
	log().debug(log().tab(), "present mode is ", presentMode, " (preferred present mode is mode ", VK_PRESENT_MODE_MAILBOX_KHR, ")");

	// set the surface capabilities if something went wrong
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	check_surface_capabilities(surfaceCapabilities);

	// create the extent
	create_swapchain_extent(surfaceCapabilities);

	// create the swapchain
	const auto cond = (Application::renderSystem.device.graphicsQueue().familyIndex != Application::renderSystem.device.presentQueue().familyIndex);

	VkSwapchainCreateInfoKHR createInfo = {
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		0,
		m_surface,
		surfaceCapabilities.minImageCount + 1,
		format.format,
		format.colorSpace,
		m_extent,
		1,
		surfaceCapabilities.supportedUsageFlags,
		(cond) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
		static_cast<uint32>((cond) ? 2 : 0),
		(cond) ? &Application::renderSystem.device.graphicsQueue().familyIndex : 0,
		surfaceCapabilities.currentTransform,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		presentMode,
		VK_TRUE,
		(m_oldSwapchain != nullptr) ? *m_oldSwapchain : VK_NULL_HANDLE
	};

	m_swapchain = vk::Swapchain(Application::renderSystem.device.device(), createInfo);

	create_swapchain_images();
	create_color_resources();
	create_depth_buffer();
}

} // namespace vulkan

} // namespace lyra
