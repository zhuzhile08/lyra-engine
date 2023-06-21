/*************************
 * @file VulkanRenderSystem.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief The implementation for all vulkan backend structures in their raw forms
 * 
 * @date 2023-06-04
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>

#include <vector>
#include <Common/Array.h>
#include <string>

#include <SDL.h>
#include <vulkan/vulkan.h>
#ifdef __APPLE__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#pragma clang diagnostic ignored "-Wdeprecated-copy"
#pragma clang diagnostic ignored "-Wignored-qualifiers"
#pragma clang diagnostic ignored "-Wswitch"
#include <vk_mem_alloc.h>
#pragma clang diagnostic pop
#else
#include <vk_mem_alloc.h>
#endif
#include <Common/RAIIContainers.h>

namespace lyra {

namespace vulkan {

namespace vk {

using Instance = RAIIContainer<VkInstance, NullHandle>;
using PhysicalDevice = RAIIContainer<VkPhysicalDevice, VkInstance>;
using Device = RAIIContainer<VkDevice, VkPhysicalDevice>;
using Queue = RAIIContainer<VkQueue, VkDevice>;
using SurfaceKHR = RAIIContainer<VkSurfaceKHR, VkInstance>;
using Surface = SurfaceKHR;
using CommandPool = RAIIContainer<VkCommandPool, VkDevice>;
using SwapchainKHR = RAIIContainer<VkSwapchainKHR, VkDevice>;
using Swapchain = SwapchainKHR;
using Image = RAIIContainer<VkImage, VkDevice>;
using ImageView = RAIIContainer<VkImageView, VkDevice>;
using RenderPass = RAIIContainer<VkRenderPass, VkDevice>;
using Framebuffer = RAIIContainer<VkFramebuffer, VkDevice>;
using Semaphore = RAIIContainer<VkSemaphore, VkDevice>;
using Fence = RAIIContainer<VkFence, VkDevice>;
using Event = RAIIContainer<VkEvent, VkDevice>;
using QueryPool = RAIIContainer<VkQueryPool, VkDevice>;
using DescriptorSetLayout = RAIIContainer<VkDescriptorSetLayout, VkDevice>;
using DescriptorPool = RAIIContainer<VkDescriptorPool, VkDevice>;
using DescriptorSet = RAIIContainer<VkDescriptorSet, VkDevice>;
using ShaderModule = RAIIContainer<VkShaderModule, VkDevice>;
using PipelineLayout = RAIIContainer<VkPipelineLayout, VkDevice>;
using Pipeline = RAIIContainer<VkPipeline, VkDevice>;
using PipelineCache = RAIIContainer<VkPipelineCache, VkDevice>;
using GraphicsPipeline = Pipeline;
using ComputePipeline = Pipeline;
using Buffer = RAIIContainer<VkBuffer, VkDevice>;
using Sampler = RAIIContainer<VkSampler, VkDevice>;

} // namespace vk

namespace vma {

using Allocator = RAIIContainer<VmaAllocator, VkInstance>;
using Allocation = RAIIContainer<VmaAllocation, VmaAllocator>;
using Pool = RAIIContainer<VmaPool, VmaAllocator>;
using DefragmentationContext = RAIIContainer<VmaDefragmentationContext, VmaAllocator>;

} // namespace vma 

struct InitInfo {
	std::string title;
	Array<uint32, 3> version;
	SDL_Window* window;
	std::vector <const char*> requestedValidationLayers;
	std::vector <const char*> requestedDeviceExtensions;
};

bool init_render_system(const InitInfo& info);

} // namespace vulkan

/*

struct RenderSystem {
public:
	RenderSystem() = default;
	RenderSystem(Window* const window) : window(window) { }

	void wait_device_queue(const vulkan::vk::Queue& queue) const;
	void draw();

	NODISCARD uint8 currentFrame() const noexcept { return m_currentFrame; }
	NODISCARD uint32 imageIndex() const noexcept { return m_imageIndex; }

	vulkan::Device device;
	Array<vulkan::CommandPool, 4> commandPools;
	vulkan::Window vulkanWindow;
	Array<vulkan::Frame, Settings::RenderConfig::maxFramesInFlight> frames;

	Dynarray<Renderer*, Settings::RenderConfig::maxConcurrentRenderers> m_renderers;
	Dynarray<SmartPointer<Renderer>, Settings::RenderConfig::maxConcurrentRenderers * 2> m_deletedRenderers;

	uint8 m_currentFrame = 0;
	uint8 m_pastFrame;
	uint32 m_imageIndex;

	Window* window;

	void add_renderer(Renderer* const renderer);

	void present_device_queue();
	void submit_device_queue(const VkPipelineStageFlags& stageFlags) const;

	void update_frame_count() noexcept;
};

*/

} // namespace lyra
