#include <rendering/vulkan/vulkan_window.h>

#include <algorithm>

#include <SDL.h>
#include <SDL_vulkan.h>



#include <core/settings.h>

#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/command_buffer.h>

#include <core/application.h>

namespace lyra {

namespace vulkan {

Window::Window() {
	create_window_surface();
	create_swapchain();
	create_sync_objects();
}

Window::~Window() noexcept {
	for (int i = 0; i < m_imageAvailableSemaphores.size(); i++) { // sync objects
		vkDestroySemaphore(Application::renderSystem()->device()->device(), m_renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(Application::renderSystem()->device()->device(), m_imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(Application::renderSystem()->device()->device(), m_inFlightFences[i], nullptr);
	}
	for (uint32 i = 0; i < m_views.size(); i++) vkDestroyImageView(Application::renderSystem()->device()->device(), m_views[i], nullptr);
	vkDestroySwapchainKHR(Application::renderSystem()->device()->device(), m_swapchain, nullptr); // swapchain and old swapchain
	if (m_oldSwapchain != nullptr) vkDestroySwapchainKHR(Application::renderSystem()->device()->device(), *m_oldSwapchain, nullptr);
	vkDestroySurfaceKHR(Application::renderSystem()->device()->instance(), m_surface, nullptr); // window surface
}

void Window::recreate() {
	// wait until all commands are done executing
	vkDeviceWaitIdle(Application::renderSystem()->device()->device());

	// destroy the images
	for (uint32 i = 0; i < m_views.size(); i++) vkDestroyImageView(Application::renderSystem()->device()->device(), m_views[i], nullptr);
	m_depthImage.destroy();
	m_colorImage.destroy();
	m_depthMem.destroy();
	m_colorMem.destroy();
	// destroy the swapchain
	vkDestroySwapchainKHR(Application::renderSystem()->device()->device(), m_swapchain, nullptr);
	if (m_oldSwapchain != nullptr) vkDestroySwapchainKHR(Application::renderSystem()->device()->device(), *m_oldSwapchain, nullptr);

	// recreate the swapchain
	create_swapchain();
}

void Window::wait(const uint32& fenceIndex) const {
	vassert(Application::renderSystem()->device()->waitForFences(1, m_inFlightFences.at(fenceIndex), VK_TRUE, UINT64_MAX), "wait for Vulkan fences to finish");
}

void Window::reset(const uint32& fenceIndex) const {
	vassert(Application::renderSystem()->device()->resetFences(1, m_inFlightFences.at(fenceIndex)), "reset Vulkan fences");
}

void Window::create_swapchain_extent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) {
	int width, height;
	SDL_Vulkan_GetDrawableSize(Application::window()->get(), &width, &height);

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
	vkGetPhysicalDeviceSurfaceFormatsKHR(Application::renderSystem()->device()->physicalDevice(), m_surface, &availableFormatCount, nullptr);
	std::vector <VkSurfaceFormatKHR> availableFormats(availableFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(Application::renderSystem()->device()->physicalDevice(), m_surface, &availableFormatCount, availableFormats.data());
	// check the formats
	for (uint32 i = 0; i < availableFormats.size(); i++) {
		if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			m_format = availableFormats[i].format;
			return availableFormats[i];
		}
	}

	m_format = availableFormats[0].format;
	return availableFormats[0];
}

const VkPresentModeKHR Window::get_optimal_present_mode() const {
	uint32 availablePresentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(Application::renderSystem()->device()->physicalDevice(), m_surface, &availablePresentModeCount, nullptr);
	std::vector <VkPresentModeKHR> availablePresentModes(availablePresentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(Application::renderSystem()->device()->physicalDevice(), m_surface, &availablePresentModeCount, availablePresentModes.data());
	// check the presentation modess
	for (uint32 i = 0; i < availablePresentModes.size(); i++) {
		if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentModes[i];
			break;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

const VkSampleCountFlagBits Window::get_max_samples() const noexcept {
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(Application::renderSystem()->device()->physicalDevice(), &physicalDeviceProperties);

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
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Application::renderSystem()->device()->physicalDevice(), m_surface, &surfaceCapabilities);

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

void Window::create_window_surface() {
	// thankfully, SDL can handle the platform specific stuff for creating surfaces for me, which makes it all way easier
	lassert(SDL_Vulkan_CreateSurface(Application::window()->get(), Application::renderSystem()->device()->instance(), &m_surface) == SDL_TRUE, "Failed to create Vulkan window surface");
}

void Window::create_swapchain_images() {
	// get the number of images
	uint32 imageCount;
	vassert(vkGetSwapchainImagesKHR(Application::renderSystem()->device()->device(), m_swapchain, &imageCount, nullptr), "retrieve Vulkan swapchain images");
	m_images.resize(imageCount); m_views.resize(imageCount);
	vkGetSwapchainImagesKHR(Application::renderSystem()->device()->device(), m_swapchain, &imageCount, m_images.data());

	// I hate this bro why C++
	// this code stems from the disability to have a vector with my own image type, because then vkGetSwapchainImagesKHR won't work properly, so I had to separate everything again
	for (uint32 i = 0; i < imageCount; i++) {
		// image view creation info
		VkImageViewCreateInfo createInfo{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			nullptr,
			0,
			m_images[i],
			VK_IMAGE_VIEW_TYPE_2D,
			m_format,
			{ VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY },
			{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
		};

		// create the view
		vassert(vkCreateImageView(Application::renderSystem()->device()->device(), &createInfo, nullptr, &m_views.at(i)), "create Vulkan image views");
	}
}

void Window::create_depth_buffer() {
	m_depthBufferFormat = m_depthImage.get_best_format({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_TILING_OPTIMAL);

	// create memory and image
	vassert(Application::renderSystem()->device()->createImage(
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
		m_depthMem.get_alloc_create_info(VMA_MEMORY_USAGE_GPU_ONLY, VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)),
		m_depthImage.m_image,
		m_depthMem.m_memory
	), "create Vulkan depth buffer");

	// create the image view
	m_depthImage.create_view(VK_FORMAT_D32_SFLOAT, { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 });

	// transition the image layout
	m_depthImage.transition_layout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, m_format, { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 });
}

void Window::create_color_resources() {
	m_maxMultisamples = get_max_samples();

	// create memory and image
	vassert(Application::renderSystem()->device()->createImage(
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
		m_colorMem.get_alloc_create_info(VMA_MEMORY_USAGE_GPU_ONLY, VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)),
		m_colorImage.m_image,
		m_colorMem.m_memory
	), "create Vulkan color resources");

	// create the image view
	m_colorImage.create_view(m_format, { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
}

void Window::create_swapchain() {
	Logger::log_debug(Logger::tab(), "Swapchain configurations are: ");

	// get the optimal format
	VkSurfaceFormatKHR format = get_optimal_format();
	Logger::log_debug(Logger::tab(), "format is ", m_format, " (preferred format is format ", VK_FORMAT_B8G8R8A8_SRGB, " with color space ", VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, ")");
	
	VkPresentModeKHR presentMode = get_optimal_present_mode();
	Logger::log_debug(Logger::tab(), "present mode is ", presentMode, " (preferred present mode is mode ", VK_PRESENT_MODE_MAILBOX_KHR, ")");

	// set the surface capabilities if something went wrong
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	check_surface_capabilities(surfaceCapabilities);

	// create the extent
	create_swapchain_extent(surfaceCapabilities);

	// create the swapchain
	const auto temp = Application::renderSystem()->device()->graphicsQueue().familyIndex;
	const auto cond = (Application::renderSystem()->device()->graphicsQueue().familyIndex != Application::renderSystem()->device()->presentQueue().familyIndex);

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
		(cond) ? &temp : 0,
		surfaceCapabilities.currentTransform,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		presentMode,
		VK_TRUE,
		(m_oldSwapchain != nullptr) ? *m_oldSwapchain : VK_NULL_HANDLE
	};

	vassert(vkCreateSwapchainKHR(Application::renderSystem()->device()->device(), &createInfo, nullptr, &m_swapchain), "create Vulkan swapchain");

	create_swapchain_images();
	create_color_resources();
	create_depth_buffer();
}

void Window::create_sync_objects() {
	m_imageAvailableSemaphores.resize(Settings::Rendering::maxFramesInFlight);
	m_renderFinishedSemaphores.resize(Settings::Rendering::maxFramesInFlight);
	m_inFlightFences.resize(Settings::Rendering::maxFramesInFlight);

	VkSemaphoreCreateInfo semaphoreInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VkFenceCreateInfo fenceInfo{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		VK_FENCE_CREATE_SIGNALED_BIT
	};

	for (int i = 0; i < Settings::Rendering::maxFramesInFlight; i++) {
		vassert(vkCreateSemaphore(Application::renderSystem()->device()->device(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]),
			"create Vulkan Synchronization Objects");
		vassert(vkCreateSemaphore(Application::renderSystem()->device()->device(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]),
			"create Vulkan Synchronization Objects");
		vassert(vkCreateFence(Application::renderSystem()->device()->device(), &fenceInfo, nullptr, &m_inFlightFences[i]),
			"create Vulkan Synchronization Objects");
	}
}

} // namespace vulkan

} // namespace lyra
