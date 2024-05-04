#include <Graphics/VulkanRenderSystem.h>

#include <Common/Utility.h>
#include <Common/Logger.h>
#include <Common/Config.h>

#include <Graphics/Renderer.h>
#include <Graphics/Window.h>
#include <Graphics/Material.h>
#include <Graphics/Mesh.h>

#include <Entity/Components/Camera.h>
#include <Entity/Components/MeshRenderer.h>

#include <Resource/ResourceSystem.h>

#include <SDL_vulkan.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_sdl3.h>

#include <utility>
#include <limits>
#include <map>
#include <string>

namespace lyra {

namespace {

VKAPI_ATTR VkBool32 VKAPI_CALL validationCallBack(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
	void*
) {
	switch (messageSeverity) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			lyra::log::trace("{}\n", callbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			lyra::log::info("{}\n", callbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			lyra::log::warning("{}\n", callbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			lyra::log::error("{}\n", callbackData->pMessage);
			break;
			
		default:
			lyra::log::debug("{}\n", callbackData->pMessage);
			break;
	}

	return VK_FALSE;
}

constexpr void imguiVulkanAssert(VkResult r) {
	vulkanAssert(r, "ImGUI Vulkan: Vulkan call failed with code: {}", r);
}

}

namespace renderer {

vulkan::RenderSystem* globalRenderSystem = nullptr;

extern Window* globalWindow;

} // namespace renderer

namespace vulkan {

// render system
RenderSystem::RenderSystem(
	const Array<uint32, 3>& version,
	std::string_view defaultVertexShaderPath, 
	std::string_view defaultFragmentShaderPath
) : defaultVertexShaderPath(defaultVertexShaderPath), defaultFragmentShaderPath(defaultFragmentShaderPath) {
	{ // create instance
#ifdef __APPLE__
		SDL_setenv("MVK_CONFIG_USE_METAL_ARGUMENT_BUFFERS", "1", 1);
#endif

#ifdef CUSTOM_VULKAN_VALIDATION_LAYER
		SDL_setenv("VK_LAYER_PATH", CUSTOM_VULKAN_VALIDATION_LAYER_PATH, 1);
#endif

		// check if requested validation layers are available
#ifndef NDEBUG
		uint32 availableValidationLayerCount = 0;
		vkEnumerateInstanceLayerProperties(&availableValidationLayerCount, nullptr);
		Vector<VkLayerProperties> availableValidationLayers(availableValidationLayerCount);
		vkEnumerateInstanceLayerProperties(&availableValidationLayerCount, availableValidationLayers.data());

		// go through every requested layers and see if they are available
		for (const auto& requestedValidationLayer : config::requestedValidationLayers) {
			bool found = false;
			log::info("Available layers:");

			for (const auto& availableValidationLayer : availableValidationLayers) {
				log::debug("\t{}: {}", availableValidationLayer.layerName, availableValidationLayer.description);
				if (strcmp(requestedValidationLayer, availableValidationLayer.layerName) == 0) {
					found = true;
					break;
				}
			}

			ASSERT(found, "User required Vulkan validation layer wasn't found: {}!", requestedValidationLayer);
		}
#endif
		// get all extensions
		uint32 instanceExtensionCount;
		auto extensions = SDL_Vulkan_GetInstanceExtensions(&instanceExtensionCount);
		Vector<const char*> instanceExtensions(extensions, extensions + instanceExtensionCount);

		// add some required extensions
		instanceExtensions.pushBack("VK_KHR_get_physical_device_properties2");
#ifndef NDEBUG
		instanceExtensions.pushBack("VK_EXT_debug_utils");
#endif
#ifdef __APPLE__
		instanceExtensions.pushBack("VK_KHR_portability_enumeration");
#endif

#ifndef NDEBUG // debug messenger create info
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo {
			VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			nullptr,
			0,
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			validationCallBack,
			nullptr
		};
#endif

		// define some info for the application that will be used in instance creation
		VkApplicationInfo appInfo{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			nullptr,
			config::title.data(),
			VK_MAKE_API_VERSION(0, version[0], version[1], version[2]),
			"LyraEngine",
			VK_MAKE_API_VERSION(0, 0, 7, 0),
			VK_API_VERSION_1_3
		};

		// defining some instance info
		VkInstanceCreateInfo createInfo{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#ifndef NDEBUG
			&debugMessengerCreateInfo,
#else
			nullptr,
#endif
#ifdef _WIN32
			0,
#elif __APPLE__
			VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
#endif
			&appInfo,
#ifndef NDEBUG
			static_cast<uint32>(config::requestedValidationLayers.size()),
			config::requestedValidationLayers.data(),
#else
			0,
			nullptr,
#endif
			static_cast<uint32>(instanceExtensions.size()),
			instanceExtensions.data()
		};

		// create the instance
		instance = vk::Instance(VK_NULL_HANDLE, createInfo);

#ifndef NDEBUG
		debugMessenger = vk::DebugUtilsMessenger(instance, debugMessengerCreateInfo);
#endif
	}
	
	{ // find a suitable physical device
		// structure to hold important data to retrieve from a physical device
		struct PhysicalDeviceData {
			VkPhysicalDevice device;
			VkPhysicalDeviceProperties properties;
			VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties;
			VkPhysicalDeviceFeatures features;
			QueueFamilies queueFamilies;
		};

		// get the extended physical device properties function
		auto vkGetPhysicalDeviceProperties2KHR = (PFN_vkGetPhysicalDeviceProperties2KHR) vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2KHR");
		ASSERT(vkGetPhysicalDeviceProperties2KHR, "vkGetInstanceProcAddr() for \"vkGetPhysicalDeviceProperties2KHR\" returned a null pointer!");

		// get all devices
		uint32 deviceCount = 0;
		VULKAN_ASSERT(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr), "find any Vulkan suitable GPUs");
		Vector<VkPhysicalDevice> devices(deviceCount);			 // just put this in here cuz I was lazy
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		// a ordered map with every GPU. The one with the highest score is the one that is going to be the used GPU
		std::multimap <uint32, PhysicalDeviceData> possibleDevices;

		for (const auto& device : devices) {
			log::info("GPU {}: ", std::to_string((&device - &devices[0]) + 1));
			
			{ // rate the physical device
				// set the score to 1 first, if the GPU does not have required features, set it to 0
				int score = 0;

				QueueFamilies localQueueFamilies;

				uint32 queueFamilyCount = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
				localQueueFamilies.queueFamilyProperties.resize(queueFamilyCount);
				vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, localQueueFamilies.queueFamilyProperties.data());

				uint32 i = 0;
				for (const auto& property : localQueueFamilies.queueFamilyProperties) {
					if (
						localQueueFamilies.computeFamilyIndex == std::numeric_limits<uint32>::max() && 
						(property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					) {
						localQueueFamilies.graphicsFamilyIndex = i;
					} 
					
					if (
						localQueueFamilies.computeFamilyIndex == std::numeric_limits<uint32>::max() && 
						(property.queueFlags & VK_QUEUE_COMPUTE_BIT)
					) {
						localQueueFamilies.computeFamilyIndex = i;
					} 
					
					if (
						localQueueFamilies.copyFamilyIndex == std::numeric_limits<uint32>::max() && 
						(property.queueFlags & VK_QUEUE_TRANSFER_BIT)
					) {
						localQueueFamilies.copyFamilyIndex = i;
					}

					if (
						localQueueFamilies.copyFamilyIndex != std::numeric_limits<uint32>::max() && 
						localQueueFamilies.computeFamilyIndex != std::numeric_limits<uint32>::max() &&
						localQueueFamilies.copyFamilyIndex != std::numeric_limits<uint32>::max()
					) {
						score = 1;
						break;
					}

					i++;
				}

				if (score == 0) {
					log::warning("GPU does not have the required queue families available!");
				}
				
				// get the available extensions
				uint32 availableDeviceExtensionCount = 0;
				vkEnumerateDeviceExtensionProperties(device, nullptr, &availableDeviceExtensionCount, nullptr);
				Vector<VkExtensionProperties> availableDeviceExtensions(availableDeviceExtensionCount);
				vkEnumerateDeviceExtensionProperties(device, nullptr, &availableDeviceExtensionCount, availableDeviceExtensions.data());	

				VkPhysicalDeviceDescriptorIndexingProperties descriptorIndexingProperties {
					.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES
				};
				VkPhysicalDeviceProperties2KHR extendedProperties {
					.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR,
					.pNext = &descriptorIndexingProperties
				};
				vkGetPhysicalDeviceProperties2KHR(device, &extendedProperties);
				VkPhysicalDeviceFeatures features;
				vkGetPhysicalDeviceFeatures(device, &features);

				// some required features. If not available, make the GPU unavailable
				if ([&availableDeviceExtensions]() -> bool {
#ifndef NDEBUG
						// print all all availabe extensions
						log::info("Available device extensions:");
						for (const auto& availableDeviceExtension : availableDeviceExtensions) {
							log::debug("\t{}", availableDeviceExtension.extensionName);
						}
#endif
						UnorderedSparseSet<std::string> requestedExtensions(config::requestedDeviceExtensions.begin(), config::requestedDeviceExtensions.end());
#ifdef __APPLE__
						requestedExtensions.emplace("VK_KHR_portability_subset"); 
#endif
						// go through every requested extensions and see if they are available
						for (const auto& availableDeviceExtension : availableDeviceExtensions) {
							requestedExtensions.erase(availableDeviceExtension.extensionName);
						}

						return !requestedExtensions.empty();
					} ()
				) {
					score = 0;
					log::warning("GPU does not have some required features!");
				}

				// the actual scoring system
				if (score > 0) {
					log::info("Available device features and properties: ");

					score = 0;
					if (extendedProperties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) { // cpu type
						score += 20;
						log::debug("\t{}", "Discrete GPU");
					} if (features.samplerAnisotropy) {
						score += 4;
						log::debug("\t{}", "Supports anistropic filtering");
					} 
					score += (int)(extendedProperties.properties.limits.maxImageDimension2D / 2048);

					log::info("Score: {}\n", score);
				}

				// insert the device into the queue
				possibleDevices.emplace(score, PhysicalDeviceData {device, extendedProperties.properties, descriptorIndexingProperties, features, localQueueFamilies});
			}
		}

		if (possibleDevices.rbegin()->first <= 0) {
			ASSERT(false, "Failed to find GPU with enough features");
		} else log::info("Picked the GPU with a score of: {}!", possibleDevices.rbegin()->first);

		physicalDevice = vk::PhysicalDevice(possibleDevices.rbegin()->second.device, instance);
		deviceProperties = possibleDevices.rbegin()->second.properties;
		descriptorIndexingProperties = possibleDevices.rbegin()->second.descriptorIndexingProperties;
		deviceFeatures = possibleDevices.rbegin()->second.features;
		queueFamilies = possibleDevices.rbegin()->second.queueFamilies;
	}

	{ // create logical device
		Vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		UnorderedSparseSet <uint32> uniqueQueueFamilies { queueFamilies.graphicsFamilyIndex, queueFamilies.computeFamilyIndex, queueFamilies.copyFamilyIndex };
		
		float32 queuePriority = 1.0f;
		for (const auto& queueFamily : uniqueQueueFamilies) {
			queueCreateInfos.pushBack({
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				nullptr,
				0,
				queueFamily,
				1,
				&queuePriority
			});
		}

#ifdef __APPLE__
		Dynarray<const char*, config::requestedDeviceExtensions.size() + 1> requestedExtensions(config::requestedDeviceExtensions.begin(), config::requestedDeviceExtensions.end());
		requestedExtensions.pushBack("VK_KHR_portability_subset");
#else
		auto& requestedExtensions = config::requestedDeviceExtensions;
#endif

		VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptorIndexingFeatures { 
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
			.shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
			.descriptorBindingPartiallyBound = VK_TRUE,
			.descriptorBindingVariableDescriptorCount = VK_TRUE,
			.runtimeDescriptorArray = VK_TRUE
		};

		// device creation info
		VkDeviceCreateInfo createInfo {
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			&descriptorIndexingFeatures,
			0,
			static_cast<uint32>(queueCreateInfos.size()),
			queueCreateInfos.data(),
#ifndef NDEBUG
			static_cast<uint32>(config::requestedValidationLayers.size()),
			config::requestedValidationLayers.data(),
#else
			0,
			nullptr,
#endif
			static_cast<uint32>(requestedExtensions.size()),
			requestedExtensions.data(),
			&deviceFeatures
		};
		// create the device
		device = vk::Device(physicalDevice, createInfo);
	}

	{ // create queues
		graphicsQueue = vk::Queue(device, queueFamilies.graphicsFamilyIndex, 0);
		computeQueue = vk::Queue(device, queueFamilies.computeFamilyIndex, 0);
		copyQueue = vk::Queue(device, queueFamilies.computeFamilyIndex, 0);
	}
	
	{ // create the memory allocator
		// creation info
		VmaAllocatorCreateInfo createInfo {
			0,
			physicalDevice,
			device,
			0,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			instance,
			VK_API_VERSION_1_3,
			nullptr
		};

		// create the allocator
		allocator = vma::Allocator(instance, createInfo);
	}
}

void RenderSystem::initRenderComponents() {
	commandQueue = commandQueue.create();
	swapchain = swapchain.create(*commandQueue);
	descriptorPools = descriptorPools.create( Vector<DescriptorPools::Size> {
		{ DescriptorSets::Type::sampler, 1 },
		{ DescriptorSets::Type::imageSampler, 8 },
		{ DescriptorSets::Type::sampledImage, 8 },
		{ DescriptorSets::Type::storageImage, 2 },
		{ DescriptorSets::Type::texelBuffer, 1 },
		{ DescriptorSets::Type::texelStorageBuffer, 1 },
		{ DescriptorSets::Type::uniformBuffer, 4 },
		{ DescriptorSets::Type::storageBuffer, 4 },
		{ DescriptorSets::Type::dynamicUniformBuffer, 2 },
		{ DescriptorSets::Type::dynamicStorageBuffer, 2 },
		{ DescriptorSets::Type::inputAttachment, 1 }
	}, DescriptorPools::Flags::freeDescriptorSet | DescriptorPools::Flags::updateAfterBind);

	defaultVertexShader = &resource::shader(defaultVertexShaderPath);
	defaultFragmentShader = &resource::shader(defaultFragmentShaderPath);

	Vector<const Image*> swapchainImages;
	swapchainImages.reserve(swapchain->images.size());

	for (const auto& image : swapchain->images)
		swapchainImages.pushBack(&image);

	renderTargets.pushBack(new RenderTarget({
		{
			{ &swapchain->colorImage },
			RenderTarget::Attachment::Type::color,
			Image::Layout::colorAttachment,
			Image::Layout::undefined,
			Image::Layout::colorAttachment,
			Image::Aspect::color,
			0,
			{ 
				GPUMemory::LoadMode::clear,
				GPUMemory::StoreMode::store,
				GPUMemory::LoadMode::dontCare,
				GPUMemory::StoreMode::dontCare
			},
			{
				true,
				VK_SUBPASS_EXTERNAL,
				Pipeline::Stage::colorAttachmentOutput | Pipeline::Stage::earlyFragmentTests,
				Pipeline::Stage::colorAttachmentOutput | Pipeline::Stage::earlyFragmentTests,
				GPUMemory::Access::none,
				GPUMemory::Access::colorAttachmentWrite | GPUMemory::Access::depthStencilAttachmentWrite
			}
		},
		{
			{ &swapchain->depthImage },
			RenderTarget::Attachment::Type::depth,
			Image::Layout::depthStencilAttachment,
			Image::Layout::undefined,
			Image::Layout::depthStencilAttachment,
			Image::Aspect::depth,
			0,
			{ 
				GPUMemory::LoadMode::clear,
				GPUMemory::StoreMode::dontCare,
				GPUMemory::LoadMode::dontCare,
				GPUMemory::StoreMode::dontCare
			}
		},
		{
			swapchainImages,
			RenderTarget::Attachment::Type::render,
			Image::Layout::colorAttachment,
			Image::Layout::undefined,
			Image::Layout::present,
			Image::Aspect::color,
			0,
			{ 
				GPUMemory::LoadMode::dontCare,
				GPUMemory::StoreMode::store,
				GPUMemory::LoadMode::dontCare,
				GPUMemory::StoreMode::dontCare
			}
		}
	}));
	defaultRenderTarget = renderTargets.back();

	defaultGraphicsProgram = new GraphicsProgram();
	graphicsPrograms.emplace(defaultGraphicsProgram->hash, defaultGraphicsProgram);
	graphicsPipelines.emplace(GraphicsPipeline::Builder().hash(), new GraphicsPipeline());
}


CommandQueue::CommandPool::CommandPool() {
	VkCommandPoolCreateInfo createInfo{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		renderer::globalRenderSystem->queueFamilies.graphicsFamilyIndex
	};

	commandPool = vk::CommandPool(renderer::globalRenderSystem->device, createInfo);
}

void CommandQueue::CommandPool::reset() {
	VULKAN_ASSERT(renderer::globalRenderSystem->resetCommandPool(commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT), "reset command pool");
}

CommandQueue::CommandBuffer::CommandBuffer(const CommandPool& commandPool, const VkCommandBufferLevel& level) : 
	commandPool(commandPool.commandPool) {
	// locate the memory
	VkCommandBufferAllocateInfo allocInfo{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		this->commandPool,
		level,
		1
	};

	// create the command buffers
	commandBuffer = vk::CommandBuffer(renderer::globalRenderSystem->device.get(), allocInfo);
}

void CommandQueue::CommandBuffer::begin(const Usage& usage) const {
	// some info about the recording
	VkCommandBufferBeginInfo beginInfo{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		static_cast<VkCommandBufferUsageFlags>(usage),
		nullptr
	};

	// start recording
	VULKAN_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo), "start recording Vulkan command buffer");
}

void CommandQueue::CommandBuffer::reset(VkCommandBufferResetFlags flags) const {
	VULKAN_ASSERT(vkResetCommandBuffer(commandBuffer, flags), "reset command buffer"); // reset the command buffer
}

CommandQueue::CommandQueue() { 
	commandPools.resize(config::maxFramesInFlight);
}

void CommandQueue::reset() {
	auto newCommandQueue = CommandQueue();
	std::swap(*this, newCommandQueue);
}

void CommandQueue::submit(VkFence fence, bool wait) {
	if (activeCommandBuffer != nullptr && queue != VK_NULL_HANDLE) {
		if (pipelineStageFlags.empty()) {
			pipelineStageFlags.pushBack(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
		}

		VkSubmitInfo submitInfo {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			static_cast<uint32>(waitSemaphores.size()),
			waitSemaphores.data(),
			pipelineStageFlags.data(),
			1,
			&activeCommandBuffer->commandBuffer.get(),
			static_cast<uint32>(signalSemaphores.size()),
			signalSemaphores.data()
		};

		VULKAN_ASSERT(vkQueueSubmit(queue, 1, &submitInfo, fence), "submit Vulkan queue");

		if (wait && fence != VK_NULL_HANDLE) {
			VULKAN_ASSERT(renderer::globalRenderSystem->waitForFence(vk::Fence(fence, VK_NULL_HANDLE), VK_TRUE, std::numeric_limits<uint32>::max()), "wait for fence to finish");
		}

		activeCommandBuffer = nullptr;

		currentFrame = (currentFrame + 1) % config::maxFramesInFlight;
	}

	waitSemaphores.clear();
	signalSemaphores.clear();
	pipelineStageFlags.clear();
}

void CommandQueue::oneTimeBegin() {
	activeCommandBuffer = new CommandBuffer(commandPools.at(currentFrame));
	activeCommandBuffer->begin(CommandQueue::CommandBuffer::Usage::oneTimeSubmit);
}

void CommandQueue::oneTimeSubmit() {
	activeCommandBuffer->end();

	VkSubmitInfo submitInfo {
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		0,
		nullptr,
		nullptr,
		1,
		&activeCommandBuffer->commandBuffer.get(),
		0,
		nullptr
	};

	VULKAN_ASSERT(vkQueueSubmit(renderer::globalRenderSystem->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE), "submit Vulkan queue");
	vkQueueWaitIdle(renderer::globalRenderSystem->graphicsQueue);

	delete activeCommandBuffer;
}

GPUBuffer::GPUBuffer(
	VkDeviceSize size,
	VkBufferUsageFlags bufferUsage, 
	VmaMemoryUsage memUsage
) : size(size) {
	VkBufferCreateInfo createInfo{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		nullptr,
		0,
		size,
		bufferUsage,
		VK_SHARING_MODE_EXCLUSIVE,
		0,
		0
	};

	buffer = vk::Buffer(
		renderer::globalRenderSystem->device, 
		renderer::globalRenderSystem->allocator, 
		createInfo, 
		getAllocCreateInfo(memUsage), 
		memory
	);
}

void GPUBuffer::copyData(const void* src, size_type copySize) {
	void* data { };
	VULKAN_ASSERT(renderer::globalRenderSystem->mapMemory(memory, &data), "map buffer memory at {}", getAddress(memory));
	
	memcpy(data, src, (copySize == 0) ? static_cast<size_type>(size) : copySize);

	renderer::globalRenderSystem->unmapMemory(memory);
}

void GPUBuffer::copyData(const void** src, uint32 arraySize, size_type elementSize) {
	char* data { };
	VULKAN_ASSERT(renderer::globalRenderSystem->mapMemory(memory, (void**)&data), "map buffer memory at {}", getAddress(memory));

	for (uint32 i = 0; i < arraySize; i++) {
		memcpy(static_cast<void*>(data + elementSize * i), src[i], elementSize);
	}

	renderer::globalRenderSystem->unmapMemory(memory);
}

void GPUBuffer::copy(const GPUBuffer& srcBuffer) {
	CommandQueue commandQueue;
	commandQueue.oneTimeBegin();

	VkBufferCopy copyRegion{
		0,
		0,
		size
	};
	commandQueue.activeCommandBuffer->copyBuffer(srcBuffer.buffer, buffer, copyRegion);

	commandQueue.oneTimeSubmit();
}

Image::Resource::Resource(
	const Image& image,
	const VkImageSubresourceRange& subresourceRange, 
	Type viewType, 
	Format format, 
	const VkComponentMapping& colorComponents
) : image(&image) {
	VkImageViewCreateInfo createInfo{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0,
		image.image,
		static_cast<VkImageViewType>(viewType),
		static_cast<VkFormat>((format == Format::max) ? image.format : format),
		colorComponents,
		subresourceRange
	};

	view = vk::ImageView(renderer::globalRenderSystem->device, createInfo);
}

Image::Image(const VkImageCreateInfo& info, const VmaAllocationCreateInfo& allocInfo, vma::Allocation& memory) : 
	image(
		renderer::globalRenderSystem->device,
		renderer::globalRenderSystem->allocator,
		info,
		allocInfo,
		memory
	),
	format(static_cast<Image::Format>(info.format)),
	samples(static_cast<Image::SampleCount>(info.samples)),
	tiling(static_cast<Image::Tiling>(info.tiling)) { }

void Image::transitionLayout(Image::Layout oldLayout, Image::Layout newLayout, const VkImageSubresourceRange& subresourceRange) const {
	// goofy ahh check to see which image layout to use
	VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	GPUMemory::Access sourceAccess = GPUMemory::Access::none;
	GPUMemory::Access destinationAccess = GPUMemory::Access::transferWrite;

	if (oldLayout == Image::Layout::undefined && newLayout == Image::Layout::transferDst) {
		sourceAccess = GPUMemory::Access::none; 
		destinationAccess = GPUMemory::Access::transferWrite;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; 
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == Image::Layout::transferDst && newLayout == Image::Layout::shaderReadOnly) {
		sourceAccess = GPUMemory::Access::transferWrite; 
		destinationAccess = GPUMemory::Access::shaderRead;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT; 
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == Image::Layout::undefined && newLayout == Image::Layout::depthStencilAttachment) {
		sourceAccess = GPUMemory::Access::none; 
		destinationAccess = GPUMemory::Access::depthStencilAttachmentRead | GPUMemory::Access::depthStencilAttachmentWrite;
		
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; 
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else ASSERT(false, "Invalid image layout transition was requested whilst transitioning an image layout at: {}!", getAddress(this));

	CommandQueue commandQueue;
	commandQueue.oneTimeBegin();

	commandQueue.activeCommandBuffer->pipelineBarrier(
		sourceStage, 
		destinationStage, 
		0,
		{ },
		{ }, 
		imageMemoryBarrier(
			sourceAccess, 
			destinationAccess, 
			oldLayout, 
			newLayout, 
			subresourceRange
		)
	);

	commandQueue.oneTimeSubmit();
}

void Image::copyFromBuffer(const vulkan::GPUBuffer& stagingBuffer, const VkExtent3D& extent, uint32 layerCount) {
	CommandQueue commandQueue;
	commandQueue.oneTimeBegin();

	VkBufferImageCopy imageCopy {
		0,
		0,
		0,
		{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, layerCount },
		{ 0, 0, 0 },
		extent
	};
	commandQueue.activeCommandBuffer->copyBufferToImage(stagingBuffer.buffer, image, Image::Layout::transferDst, imageCopy);

	commandQueue.oneTimeSubmit();
}

vk::Sampler Image::createSampler(
	VkSamplerAddressMode addressModeU,
	VkSamplerAddressMode addressModeV,
	VkSamplerAddressMode addressModeW,
	VkBorderColor borderColor,
	float32 maxLod,
	float32 minLod,
	float32 mipLodBias,
	VkFilter magFilter,
	VkFilter minFilter,
	VkSamplerMipmapMode mipmapMode,
	VkCompareOp compareOp,
	VkBool32 unnormalizedCoordinates
) noexcept {
	return vk::Sampler(renderer::globalRenderSystem->device, VkSamplerCreateInfo {
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		nullptr,
		0,
		magFilter,
		minFilter,
		mipmapMode,
		addressModeU,
		addressModeV,
		addressModeW,
		mipLodBias,
		config::enableAnistropy,
		renderer::globalRenderSystem->deviceProperties.limits.maxSamplerAnisotropy * config::anistropyStrength,
		(compareOp == 0) ? VK_FALSE : VK_TRUE,
		compareOp,
		minLod,
		maxLod,
		borderColor,
		unnormalizedCoordinates
	});
}

Image::Format Image::bestFormat(const Vector<Format>& candidates, Tiling tiling, VkFormatFeatureFlags features) {
	for (const auto& candidate : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(renderer::globalRenderSystem->physicalDevice, static_cast<VkFormat>(candidate), &props);

		if (tiling == Tiling::linear && (props.linearTilingFeatures & features) == features) return candidate;
		else if (tiling == Tiling::optimal && (props.optimalTilingFeatures & features) == features) return candidate;
	}

	ASSERT(false, "Failed to find supported format out of user defined formats!");

	return Format::max;
}

VkFormatProperties Image::formatProperties(Format format) const noexcept {
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(renderer::globalRenderSystem->physicalDevice, static_cast<VkFormat>(format), &formatProperties);
	return formatProperties;
}

Swapchain::Swapchain(CommandQueue& commandQueue) : commandQueue(&commandQueue) {
	surface = vk::Surface(renderer::globalRenderSystem->instance, renderer::globalWindow->window);

	{ // check present queue compatibility
		for (uint32 i = 0; i < renderer::globalRenderSystem->queueFamilies.queueFamilyProperties.size(); i++) {
			VkBool32 presentSupport { };
			VULKAN_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(renderer::globalRenderSystem->physicalDevice, i, surface, &presentSupport),
				"check physical device queue family presentation support");
			
			if (presentFamilyIndex == std::numeric_limits<uint32>::max() && presentSupport == VK_TRUE) {
				presentFamilyIndex = i;
				presentQueue = vk::Queue(renderer::globalRenderSystem->device, presentFamilyIndex, 0);
				break;
			}
		}

		if (presentFamilyIndex == std::numeric_limits<uint32>::max()) {
			ASSERT(false, "Failed to find queue family with presentation support with physical device: {} for surface: {}!", getAddress(renderer::globalRenderSystem->physicalDevice), getAddress(surface));
		}

		this->commandQueue->queue = presentQueue.get();
	}

	{ // create syncronization objects
		VkSemaphoreCreateInfo semaphoreInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
		};

		VkFenceCreateInfo fenceInfo{
			VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			nullptr,
			VK_FENCE_CREATE_SIGNALED_BIT
		};

		for (uint32 i = 0; i < config::maxFramesInFlight; i++) {
			imageAquiredSemaphores[i] = vk::Semaphore(renderer::globalRenderSystem->device, semaphoreInfo);
			submitFinishedSemaphores[i] = vk::Semaphore(renderer::globalRenderSystem->device, semaphoreInfo);
			renderFinishedFences[i] = vk::Fence(renderer::globalRenderSystem->device, fenceInfo);
		}
	}

	createSwapchain();
	createAttachments();
}

void Swapchain::createSwapchain() {
	log::info("Swapchain configurations are: ");

	VkSurfaceFormatKHR surfaceFormat; 
	{ // determine the best format
		uint32 availableFormatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(renderer::globalRenderSystem->physicalDevice, surface, &availableFormatCount, nullptr);
		Vector<VkSurfaceFormatKHR> availableFormats(availableFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(renderer::globalRenderSystem->physicalDevice, surface, &availableFormatCount, availableFormats.data());
		// check the formats
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				surfaceFormat = availableFormat;
			}
		}

		surfaceFormat = availableFormats[0];

		log::debug("\tFormat is: {} (preferred format is format: {} with color space: {})", surfaceFormat.format, VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
	}
	
	VkPresentModeKHR presentMode;
	{ // determine the presentation mode
		uint32 availablePresentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(renderer::globalRenderSystem->physicalDevice, surface, &availablePresentModeCount, nullptr);
		Vector<VkPresentModeKHR> availablePresentModes(availablePresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(renderer::globalRenderSystem->physicalDevice, surface, &availablePresentModeCount, availablePresentModes.data());
		// check the presentation modess
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				presentMode = availablePresentMode;
			}
		}

		presentMode = VK_PRESENT_MODE_FIFO_KHR;

		log::debug("\tPresent mode is {} (preferred present mode is mode {})", presentMode, VK_PRESENT_MODE_MAILBOX_KHR);
	}

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	{ // check surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(renderer::globalRenderSystem->physicalDevice, surface, &surfaceCapabilities);

		if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) {
			surfaceCapabilities.currentExtent.width = config::windowWidth;
			log::warning("Something went wrong whilst attempting getting the swapchain width!");
		} if (surfaceCapabilities.currentExtent.height == 0xFFFFFFFF) {
			surfaceCapabilities.currentExtent.height = config::windowHeight;
			log::warning("Something went wrong whilst attempting getting the swapchain height!");
		} if (surfaceCapabilities.maxImageCount == 0xFFFFFFFF) {
			surfaceCapabilities.maxImageCount = 8;
			log::warning("Something went wrong whilst attempting getting the number of swapchain images!");
		} if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
			surfaceCapabilities.supportedUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		} if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
			surfaceCapabilities.supportedTransforms = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
	}

	{ // create the extent
		auto drawableArea = renderer::windowSize();

		extent = VkExtent2D {
			static_cast<uint32>(std::clamp(drawableArea.x, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width)),
			static_cast<uint32>(std::clamp(drawableArea.t, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height))
		};
	}

	{ // create swapchain
		VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		Vector<uint32> queueFamilies;

		if (presentFamilyIndex != renderer::globalRenderSystem->queueFamilies.graphicsFamilyIndex) {
			sharingMode = VK_SHARING_MODE_CONCURRENT;
			queueFamilies = { renderer::globalRenderSystem->queueFamilies.graphicsFamilyIndex, presentFamilyIndex };
		}

		VkSwapchainCreateInfoKHR createInfo = {
			VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			nullptr,
			0,
			surface,
			surfaceCapabilities.minImageCount + 1,
			surfaceFormat.format,
			surfaceFormat.colorSpace,
			extent,
			1,
			surfaceCapabilities.supportedUsageFlags,
			sharingMode,
			static_cast<uint32>(queueFamilies.size()),
			queueFamilies.data(),
			surfaceCapabilities.currentTransform,
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			presentMode,
			VK_TRUE,
			(oldSwapchain) ? oldSwapchain : VK_NULL_HANDLE
		};

		swapchain = vk::Swapchain(renderer::globalRenderSystem->device, createInfo);
	}

	{ // create swapchain images
		Vector<VkImage> tempImages;
		
		uint32 imageCount { };
		VULKAN_ASSERT(vkGetSwapchainImagesKHR(renderer::globalRenderSystem->device, swapchain, &imageCount, nullptr), "retrieve Vulkan swapchain images");
		images.resize(imageCount); tempImages.resize(imageCount);
		vkGetSwapchainImagesKHR(renderer::globalRenderSystem->device, swapchain, &imageCount, tempImages.data());

		for (uint32 i = 0; i < imageCount; i++) {
			images[i].image = std::move(tempImages[i]);
			images[i].format = static_cast<Image::Format>(surfaceFormat.format);
			images[i].samples = Image::SampleCount::bit1;
		}
	}
}

void Swapchain::createAttachments() {
	{ // create anti aliasing images
		uint32 sampleCount = 0;

		{ // configure the multisample
			VkSampleCountFlags counts = renderer::globalRenderSystem->deviceProperties.limits.framebufferColorSampleCounts & renderer::globalRenderSystem->deviceProperties.limits.framebufferDepthSampleCounts;

			uint32 mask = 2;
			while (counts & mask) mask <<= 1;
			sampleCount = mask >> 1;
		}

		colorImage = Image(
			colorImage.imageCreateInfo(
				images[0].format,
				{ extent.width, extent.height, 1 },
				VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				1,
				Image::Type::dim2,
				1,
				0,
				static_cast<Image::SampleCount>(sampleCount)
			),
			GPUMemory::getAllocCreateInfo(VMA_MEMORY_USAGE_GPU_ONLY, VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)),
			colorMem.memory
		);
	}
	
	{ // create depth images
		depthImage = Image(
			depthImage.imageCreateInfo(
				Image::bestFormat({ 
						Image::Format::d32SFloat,
						Image::Format::d32SFloatS8UInt,
						Image::Format::d24UNormS8UInt
					}, 
					Image::Tiling::optimal, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
				),
				{ extent.width, extent.height, 1 },
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				1,
				Image::Type::dim2,
				1,
				0,
				colorImage.samples
			),
			GPUMemory::getAllocCreateInfo(VMA_MEMORY_USAGE_GPU_ONLY, VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)),
			depthMem.memory
		);
	}
}

bool Swapchain::update(bool windowChanged) {
	if (invalidAttachments || invalidSwapchain || lostSurface || windowChanged) {
		auto flags = renderer::windowFlags();
		SDL_Event e;
		while ((flags & Window::Flags::minimized) == Window::Flags::minimized && SDL_WaitEvent(&e)) {
			flags = renderer::windowFlags();
		}

		vkDeviceWaitIdle(renderer::globalRenderSystem->device);

		depthMem.destroy();
		colorMem.destroy();

		if (invalidSwapchain || lostSurface || renderer::globalWindow->changed) {
			if (oldSwapchain != nullptr) oldSwapchain.destroy();
			oldSwapchain.swap(swapchain);

			if (lostSurface) {
				surface = vk::Surface(renderer::globalRenderSystem->instance, renderer::globalWindow->window);
			}

			createSwapchain();
		}

		createAttachments();
		for (auto& target : renderer::globalRenderSystem->renderTargets) {
			target->createFramebuffers();
		}

		lostSurface = false;
		invalidSwapchain = false;
		invalidAttachments = false;

		return true;
	}

	return false;
}

void Swapchain::aquire() {
	currentFrame = commandQueue->currentFrame;

	renderer::globalRenderSystem->waitForFence(renderFinishedFences[currentFrame], VK_TRUE, std::numeric_limits<uint64>::max());

	VkResult result = vkAcquireNextImageKHR(
		renderer::globalRenderSystem->device, 
		swapchain, 
		std::numeric_limits<uint64>::max(), 
		imageAquiredSemaphores[currentFrame], 
		VK_NULL_HANDLE, 
		&imageIndex
	);

	switch (result) {
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
			break;	
		case VK_ERROR_SURFACE_LOST_KHR:
			lostSurface = true;
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			invalidSwapchain = true;
			break;

		default:
			VULKAN_ASSERT(result, "aquire next vulkan swapchain image");
			break;
	}
}

void Swapchain::begin() {
	renderer::globalRenderSystem->resetFence(renderFinishedFences[currentFrame]);

	commandQueue->activeCommandBuffer = &(commandBuffer = lyra::vulkan::CommandQueue::CommandBuffer(commandQueue->commandPools[commandQueue->currentFrame]));
	commandQueue->waitSemaphores.pushBack(imageAquiredSemaphores[currentFrame]);
	commandQueue->signalSemaphores.pushBack(submitFinishedSemaphores[currentFrame]);
}

void Swapchain::present() {
	VkPresentInfoKHR presentInfo {
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1,
		&submitFinishedSemaphores[currentFrame].get(),
		1,
		&swapchain.get(),
		&imageIndex,
		nullptr
	};

	VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);

	switch (result) {
		case VK_SUCCESS:
			break;	
		case VK_ERROR_SURFACE_LOST_KHR:
			lostSurface = true;
			break;

		case VK_ERROR_OUT_OF_DATE_KHR:
		case VK_SUBOPTIMAL_KHR:
			invalidSwapchain = true;
			break;

		default:
			VULKAN_ASSERT(result, "present swapchain");
			break;
	}
}

RenderTarget::Framebuffer::Framebuffer(uint32 index, const RenderTarget& renderTarget, const glm::u32vec2& size) : size(size) { // create the framebuffers
	Vector<VkImageView> views;
	views.reserve(renderTarget.attachments.size());
	imageResources.reserve(renderTarget.attachments.size());

	for (const auto& attachment : renderTarget.attachments) {
		imageResources.pushBack({
			*attachment.images[index % attachment.images.size()], // guarantee that the images wrap pack on themselves
			{ 
				static_cast<VkImageAspectFlags>(attachment.aspect), 
				attachment.ranges.mipLayer, 
				attachment.ranges.mipCount, 
				attachment.ranges.arrayLayer, 
				attachment.ranges.arrayCount 
			}
		});
		views.pushBack(imageResources.back().view);
	}

	// create the framebuffer
	VkFramebufferCreateInfo createInfo{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		nullptr,
		0,
		renderTarget.renderPass,
		static_cast<uint32>(views.size()),
		views.data(),
		static_cast<uint32>(size.x),
		static_cast<uint32>(size.y),
		1
	};

	framebuffer = vk::Framebuffer(renderer::globalRenderSystem->device, createInfo);
}

RenderTarget::RenderTarget(const Vector<Attachment>& attchmts, const glm::u32vec2& size) : attachments(attchmts) { // ugly, but to avoid refering to the wrong object
	static constexpr VkAttachmentReference defaultAttachment = { VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED };

	{ // create render pass
		Vector<VkAttachmentDescription> attachmentDescriptions;
		attachmentDescriptions.reserve(attachments.size());

		Vector<Vector<VkAttachmentReference>> inputAttachments;
		Vector<Vector<VkAttachmentReference>> colorAttachments;
		Vector<Vector<VkAttachmentReference>> renderAttachments;
		Vector<VkAttachmentReference> depthAttachments;

		Vector<VkSubpassDescription> subpasses;
		Vector<VkSubpassDependency> dependencies;

		for (uint32 i = 0; i < attachments.size(); i++) {
			const auto& attachment = attachments[i];

			inputAttachments.resize(attachment.subpass + 1);
			colorAttachments.resize(attachment.subpass + 1);
			renderAttachments.resize(attachment.subpass + 1);
			subpasses.resize(attachment.subpass + 1);

			attachmentDescriptions.pushBack({
				0,
				static_cast<VkFormat>(attachment.images[0]->format),
				static_cast<VkSampleCountFlagBits>(attachment.images[0]->samples),
				static_cast<VkAttachmentLoadOp>(attachment.memoryModes.load),
				static_cast<VkAttachmentStoreOp>(attachment.memoryModes.store),
				static_cast<VkAttachmentLoadOp>(attachment.memoryModes.stencilLoad),
				static_cast<VkAttachmentStoreOp>(attachment.memoryModes.stencilStore),
				static_cast<VkImageLayout>(attachment.initialLayout),
				static_cast<VkImageLayout>(attachment.finalLayout)
			});

			if (attachment.dependencies.enabled) dependencies.pushBack({
				attachment.dependencies.dependentPass,
				attachment.subpass,
				static_cast<VkPipelineStageFlags>(attachment.dependencies.stageDependencies),
				static_cast<VkPipelineStageFlags>(attachment.dependencies.stageWait),
				static_cast<VkAccessFlags>(attachment.dependencies.memDependencies),
				static_cast<VkAccessFlags>(attachment.dependencies.memWait),
				0
			});

			Vector<VkAttachmentReference>* references;

			// add the attachment references
			if (attachment.type == Attachment::Type::input) {
				references = &inputAttachments[attachment.subpass];
			} else if (attachment.type == Attachment::Type::color) {
				references = &colorAttachments[attachment.subpass];
			} else if (attachment.type == Attachment::Type::render) {
				references = &renderAttachments[attachment.subpass];
			} else if (attachment.type == Attachment::Type::depth) {
				references = &depthAttachments;
			} else references = nullptr;

			references->pushBack({ i, static_cast<VkImageLayout>(attachment.layout) });
		}

		for (uint32 i = 0; i < subpasses.size(); i++) {
			// guarantee that the color and resolve attachments are the same size
			auto largerSize = std::max(colorAttachments[i].size(), renderAttachments[i].size());
			colorAttachments[i].resize(largerSize, defaultAttachment);
			renderAttachments[i].resize(largerSize, defaultAttachment);

			subpasses[i] = {
				.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
				.inputAttachmentCount = static_cast<uint32>(inputAttachments[i].size()),
				.pInputAttachments = inputAttachments[i].data(),
				.colorAttachmentCount = static_cast<uint32>(colorAttachments[i].size()),
				.pColorAttachments = colorAttachments[i].data(),
				.pResolveAttachments = renderAttachments[i].data(),
				.pDepthStencilAttachment = ((depthAttachments.size() > i) ? &depthAttachments[i] : nullptr)
			};
		}

		VkRenderPassCreateInfo createInfo {
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			nullptr,
			0,
			static_cast<uint32>(attachmentDescriptions.size()),
			attachmentDescriptions.data(),
			static_cast<uint32>(subpasses.size()),
			subpasses.data(),
			static_cast<uint32>(dependencies.size()),
			dependencies.data()
		};

		renderPass = vk::RenderPass(renderer::globalRenderSystem->device, createInfo);
	}

	createFramebuffers(size);
}

RenderTarget::~RenderTarget() {
	auto& fb = renderer::globalRenderSystem->renderTargets;
	fb.erase(std::remove(fb.begin(), fb.end(), this), fb.end());
}

void RenderTarget::createFramebuffers(const glm::u32vec2& size) {
	framebuffers.resize(attachments.back().images.size()); // there will be always as many framebuffers as images in the final attachment

	auto s = size;

	if (size.x == std::numeric_limits<uint32>::max() || size.y == std::numeric_limits<uint32>::max()) {
		s = {
			renderer::globalRenderSystem->swapchain->extent.width, 
			renderer::globalRenderSystem->swapchain->extent.height
		};
	}

	for (uint32 i = 0; i < framebuffers.size(); i++) framebuffers[i] = Framebuffer(i, *this, s);
}

void RenderTarget::begin() const {
	static constexpr Array<VkClearValue, 2> clear {{
		{
			{{ 0.0f, 0.0f, 0.0f, 1.0f }},
		},
		{
			{{ 1.0f, 0 }}
		}
	}};

	VkRenderPassBeginInfo beginInfo{
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		nullptr,
		renderPass,
		framebuffers[renderer::globalRenderSystem->swapchain->imageIndex].framebuffer,
		{	// rendering area
			{ 0, 0 },
			renderer::globalRenderSystem->swapchain->extent
		},
		clear.size(),
		clear.data()
	};

	renderer::globalRenderSystem->commandQueue->activeCommandBuffer->beginRenderPass(beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderTarget::end() const {
	renderer::globalRenderSystem->commandQueue->activeCommandBuffer->endRenderPass();
}

DescriptorSets::~DescriptorSets() {
	for (uint32 i = 0; i < descriptorSets.size(); i++) 
		vkFreeDescriptorSets(renderer::globalRenderSystem->device, descriptorSets[i].owner(), 1, &descriptorSets[i].get());
			
	renderer::globalRenderSystem->descriptorPools->allocationIndex = 0;
}

void DescriptorSets::update(uint32 index) {
	if (dirty) {
		writes.reserve(imageWrites.size() + bufferWrites.size());

		for (const auto& write : imageWrites) {
			writes.pushBack({
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				VK_NULL_HANDLE,
				write.binding,
				0,
				static_cast<uint32>(write.infos.size()),
				static_cast<VkDescriptorType>(write.type),
				write.infos.data(),
				nullptr,
				nullptr
			});
		}

		for (const auto& write : bufferWrites) {
			writes.pushBack({
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				VK_NULL_HANDLE,
				write.binding,
				0,
				static_cast<uint32>(write.infos.size()),
				static_cast<VkDescriptorType>(write.type),
				nullptr,
				write.infos.data(),
				nullptr
			});
		}
	}

	if (index != std::numeric_limits<uint32>::max()) {
		for (auto& write : writes)
			write.dstSet = descriptorSets[index];

		renderer::globalRenderSystem->updateDescriptorSet(writes);
	} else {
		for (uint32 i = 0; i < descriptorSets.size(); i++) {
			for (auto& write : writes)
				write.dstSet = descriptorSets[i];

			renderer::globalRenderSystem->updateDescriptorSet(writes);
		}
	}
}

void DescriptorSets::addDescriptorSets(uint32 count) {
	for (uint32 i = 0; i < count; i++) {
		descriptorSets.emplaceBack(
			renderer::globalRenderSystem->descriptorPools->allocate(*graphicsProgram, layoutIndex, variableCount)
		);
	}

	update();
}

void DescriptorSets::bind(uint32 index) {
	if (descriptorSets.size() <= index) {
		addDescriptorSets(1);
	}

	renderer::globalRenderSystem->commandQueue->activeCommandBuffer->bindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsProgram->pipelineLayout, layoutIndex, descriptorSets[index]);
}

DescriptorPools::DescriptorPools(const Vector<Size>& sizes, Flags flags) {
	constexpr static uint32 descriptorCount = config::descriptorPoolAllocCount * config::maxFramesInFlight;

	for (const auto& size : sizes) {
		this->sizes.pushBack({
			static_cast<VkDescriptorType>(size.type),
			descriptorCount * size.multiplier
		});
	}

	createInfo = VkDescriptorPoolCreateInfo {
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		nullptr,
		static_cast<VkDescriptorPoolCreateFlags>(flags),
		config::maxDescriptorPoolSets,
		static_cast<uint32>(sizes.size()),
		this->sizes.data()
	};

	descriptorPools.emplaceBack(renderer::globalRenderSystem->device, createInfo);
}

void DescriptorPools::reset() {
	allocationIndex = 0;

	for (auto& pool : descriptorPools)
		renderer::globalRenderSystem->resetDescriptorPool(pool, 0);
}

vk::DescriptorSet DescriptorPools::allocate(const GraphicsProgram& program, uint32 layoutIndex, bool variableCount) {
	VkDescriptorSetVariableDescriptorCountAllocateInfo variableCountAlloc {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO,
		nullptr,
		1,
		&program.dynamicDescriptorCounts[layoutIndex]
	};

	VkDescriptorSetAllocateInfo allocInfo {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		(variableCount) ? &variableCountAlloc : nullptr,
		descriptorPools[allocationIndex],
		1,
		&program.descriptorSetLayouts[layoutIndex].get()
	};

	VkResult r;
	vk::DescriptorSet descriptorSet = vk::DescriptorSet(renderer::globalRenderSystem->device, allocInfo, r);

	while (descriptorSet.get() == VK_NULL_HANDLE) {
		switch (r) {
			case VK_ERROR_FRAGMENTED_POOL:
			case VK_ERROR_OUT_OF_POOL_MEMORY:
				++allocationIndex;
				
				if (allocationIndex >= descriptorPools.size()) {
					allocInfo.descriptorPool = descriptorPools.emplaceBack(renderer::globalRenderSystem->device, createInfo).get();
				} else {
					allocInfo.descriptorPool = descriptorPools[allocationIndex];
				}

				descriptorSet = vk::DescriptorSet(renderer::globalRenderSystem->device, allocInfo, r);

				break;

			default:
				VULKAN_ASSERT(r, "allocate Vulkan descriptor set");
		}
	}

	return descriptorSet;
}

Shader::Shader(Type type, Vector<char>&& source) : shaderSrc(source), type(type) {
	VkShaderModuleCreateInfo createInfo{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		shaderSrc.size(),
		reinterpret_cast<const uint32*>(shaderSrc.data())
	};

	module = vk::ShaderModule(renderer::globalRenderSystem->device, createInfo);
}

Shader::Shader(Type type, const Vector<char>& source) : shaderSrc(source), type(type) {
	VkShaderModuleCreateInfo createInfo{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		shaderSrc.size(),
		reinterpret_cast<const uint32*>(shaderSrc.data())
	};

	module = vk::ShaderModule(renderer::globalRenderSystem->device, createInfo);
}

std::string GraphicsProgram::Builder::hash() const noexcept {
	return 
		std::string(m_bindingHash) + 
		m_pushConstantHash + 
		std::to_string(reinterpret_cast<uintptr>((m_vertexShader) ? m_vertexShader : renderer::globalRenderSystem->defaultVertexShader)) + 
		std::to_string(reinterpret_cast<uintptr>((m_fragmentShader) ? m_fragmentShader : renderer::globalRenderSystem->defaultFragmentShader));
}

GraphicsProgram::GraphicsProgram() : 
	dynamicDescriptorCounts({ config::maxDynamicBindings, 0 }),
	vertexShader(renderer::globalRenderSystem->defaultVertexShader), 
	fragmentShader(renderer::globalRenderSystem->defaultFragmentShader), 
	hash(Builder().hash()) {
	static constexpr Array<VkDescriptorBindingFlags, 8> bindingFlags = {{ 
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT
	}};
	static constexpr Array<VkDescriptorSetLayoutBindingFlagsCreateInfo, 1> bindingExt {{
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
			nullptr,
			bindingFlags.size(),
			bindingFlags.data()
		}
	}};
	static constexpr Array<Dynarray<VkDescriptorSetLayoutBinding, 8>, 1> bindings {{
		{{
			{
				0,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				1,
				VK_SHADER_STAGE_VERTEX_BIT,
				nullptr
			},
			{ 
				1,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				1,
				VK_SHADER_STAGE_VERTEX_BIT,
				nullptr
			},
			{ 
				2,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				1,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			},
			{ 
				3,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				1,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			},
			{ 
				4,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				1,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			},
			{
				5,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				1,
				VK_SHADER_STAGE_VERTEX_BIT,
				nullptr
			},
			{ 
				6,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				1,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			},
			{ 
				7,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				config::maxDynamicBindings,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			}
		}}
	}};
	static constexpr Array<VkPushConstantRange, bindings.size()> pushConstants {{
		{
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(Camera::TransformData)
		}
	}};

	Array<VkDescriptorSetLayout, bindings.size()> tmpLayouts;

	descriptorSetLayouts.resize(bindings.size());

	for (uint32 i = 0; i < bindings.size(); i++) {
		VkDescriptorSetLayoutCreateInfo createInfo{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			&bindingExt[i],
			0,
			static_cast<uint32>(bindings[i].size()),
			bindings[i].data()
		};

		if (i == 0) {
			createInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

			if (config::maxDynamicBindings >= renderer::globalRenderSystem->descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSamplers - 3) {
				auto data = bindings[i];
				data[7].descriptorCount = (dynamicDescriptorCounts[0] = renderer::globalRenderSystem->descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSamplers - 3);
				createInfo.pBindings = data.data();
			}
		}

		tmpLayouts[i] = (descriptorSetLayouts[i] = vk::DescriptorSetLayout(renderer::globalRenderSystem->device, createInfo)).get();
	}

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		tmpLayouts.size(),
		tmpLayouts.data(),
		pushConstants.size(),
		pushConstants.data()
	};

	pipelineLayout = vk::PipelineLayout(renderer::globalRenderSystem->device, pipelineLayoutCreateInfo);
}

GraphicsProgram::GraphicsProgram(const Builder& builder) : 
	vertexShader(builder.m_vertexShader), 
	fragmentShader(builder.m_fragmentShader), 
	hash(builder.hash()) {
	auto setCount = builder.m_bindings.size();

	Vector<VkDescriptorSetLayout> tmpLayouts(setCount);

	descriptorSetLayouts.resize(setCount);
	dynamicDescriptorCounts.resize(setCount);

	for (uint32 i = 0; i < setCount; i++) {
		VkDescriptorSetLayoutCreateInfo createInfo {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			&builder.m_bindingFlagsCreateInfo[i],
			0,
			static_cast<uint32>(builder.m_bindings[i].size()),
			builder.m_bindings[i].data()
		};

		dynamicDescriptorCounts[i] = builder.m_bindingFlagsCreateInfo[i].bindingCount;

		tmpLayouts[i] = (descriptorSetLayouts[i] = vk::DescriptorSetLayout(renderer::globalRenderSystem->device, createInfo)).get();
	}

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32>(setCount),
		tmpLayouts.data(),
		static_cast<uint32>(builder.m_pushConstants.size()),
		builder.m_pushConstants.data()
	};

	pipelineLayout = vk::PipelineLayout(renderer::globalRenderSystem->device, pipelineLayoutCreateInfo);
}

std::string GraphicsPipeline::Builder::hash() const noexcept {
	return 
		std::to_string(std::holds_alternative<bool>(m_viewport)) + 
		std::to_string(std::holds_alternative<bool>(m_scissor)) +
		std::to_string(m_topology) + 
		std::to_string(m_renderMode) + 
		std::to_string(m_polyFrontFace) + 
		std::to_string(m_sampleCount) + 
		(
			std::holds_alternative<bool>(m_sampleShading) ? 
				std::to_string(std::get<bool>(m_sampleShading)) : 
				std::to_string(std::get<float32>(m_sampleShading))
		) + 
		(std::holds_alternative<bool>(m_depthStencil) ? 
			std::to_string(std::get<bool>(m_sampleShading)) : 
			(
				std::to_string(std::get<Builder::DepthStencil>(m_depthStencil).write) + 
					std::to_string(std::get<Builder::DepthStencil>(m_depthStencil).compare)
			)
		) + 
		std::to_string(m_blendAttachments.size()) +
		std::to_string(reinterpret_cast<uintptr>((m_renderTarget) ? m_renderTarget : renderer::globalRenderSystem->defaultRenderTarget)) + 
		std::to_string(reinterpret_cast<uintptr>((m_graphicsProgram) ? m_graphicsProgram : renderer::globalRenderSystem->defaultGraphicsProgram));
}

GraphicsPipeline::GraphicsPipeline() : 
	Pipeline(
		renderer::globalRenderSystem->defaultRenderTarget,
		Builder().hash()
	), 
	dynamicViewport(VkViewport()),
	dynamicScissor(VkRect2D()),
	program(renderer::globalRenderSystem->defaultGraphicsProgram) {
	static constexpr VkVertexInputBindingDescription bindingDescription = Mesh::Vertex::bindingDescription();
	static constexpr Array<VkVertexInputAttributeDescription, 4> attributeDescriptions = Mesh::Vertex::attributeDescriptions();
	static constexpr VkPipelineVertexInputStateCreateInfo vertexInputInfo {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0,
		1,
		&bindingDescription,
		4,
		attributeDescriptions.data()
	};
	static constexpr VkPipelineInputAssemblyStateCreateInfo inputAssembly {
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VK_FALSE
	};
	static constexpr VkPipelineRasterizationStateCreateInfo rasterizer {
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,
		VK_FALSE,
		VK_POLYGON_MODE_FILL,
		VK_CULL_MODE_BACK_BIT,
		VK_FRONT_FACE_COUNTER_CLOCKWISE,
		VK_FALSE,
		0.0f,
		0.0f,
		0.0f,
		1.0f
	};
	static constexpr VkPipelineDepthStencilStateCreateInfo depthStencilState {
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_TRUE,
		VK_TRUE,
		VK_COMPARE_OP_LESS,
		VK_FALSE,
		VK_FALSE,
		{ },
		{ },
		0.0f,
		0.0f
	};
	static constexpr VkPipelineColorBlendAttachmentState colorBlendAttachment {
		VK_FALSE,
		VK_BLEND_FACTOR_SRC_ALPHA,
		VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};
	static constexpr VkPipelineColorBlendStateCreateInfo colorBlending {
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,
		VK_LOGIC_OP_COPY,
		1,
		&colorBlendAttachment,
		{ 0.0f, 0.0f, 0.0f, 0.0f }
	};
	static constexpr VkPipelineViewportStateCreateInfo viewportState {
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		nullptr,
		0,
		1,
		nullptr,
		1,
		nullptr
	};
	static constexpr Array<VkDynamicState, 2> dynamicState = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	static constexpr VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo {
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32>(dynamicState.size()),
		dynamicState.data()
	};

	Array<VkPipelineShaderStageCreateInfo, 2> tmpShaders = { 
		renderer::globalRenderSystem->defaultGraphicsProgram->vertexShader->stageCreateInfo(), 
		renderer::globalRenderSystem->defaultGraphicsProgram->fragmentShader->stageCreateInfo() 
	};

	VkPipelineMultisampleStateCreateInfo multisampling {
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		nullptr,
		0,
		static_cast<VkSampleCountFlagBits>(renderer::globalRenderSystem->swapchain->colorImage.samples),
		VK_FALSE,
		0.0f,
		nullptr,
		VK_FALSE,
		VK_FALSE
	};

	VkGraphicsPipelineCreateInfo createInfo {
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,	
		nullptr,
		0,
		2,
		tmpShaders.data(),
		&vertexInputInfo,
		&inputAssembly,
		nullptr,
		&viewportState,
		&rasterizer,
		&multisampling,
		&depthStencilState,
		&colorBlending,
		&dynamicStateCreateInfo,
		program->pipelineLayout,
		renderTarget->renderPass,
		0,
		VK_NULL_HANDLE,
		0
	};

	pipeline = vk::GraphicsPipeline(renderer::globalRenderSystem->device, renderer::globalRenderSystem->pipelineCache, createInfo);
}

GraphicsPipeline::GraphicsPipeline(const Builder& builder) : 
	Pipeline(
		((builder.m_renderTarget) ? builder.m_renderTarget : renderer::globalRenderSystem->defaultRenderTarget),
		builder.hash()
	), 
	program((builder.m_graphicsProgram) ? builder.m_graphicsProgram : renderer::globalRenderSystem->defaultGraphicsProgram) {
	static constexpr VkVertexInputBindingDescription bindingDescription = Mesh::Vertex::bindingDescription();
	static constexpr Array<VkVertexInputAttributeDescription, 4> attributeDescriptions = Mesh::Vertex::attributeDescriptions();
	static constexpr VkPipelineVertexInputStateCreateInfo vertexInputInfo {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0,
		1,
		&bindingDescription,
		static_cast<uint32>(attributeDescriptions.size()),
		attributeDescriptions.data()
	};

	// create some creation infos which aren't yet present

	// vertex input
	VkPipelineInputAssemblyStateCreateInfo inputAssembly {
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0,
		static_cast<VkPrimitiveTopology>(builder.m_topology),
		VK_FALSE
	};
	// rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer {
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,
		VK_FALSE,
		static_cast<VkPolygonMode>(builder.m_renderMode),
		static_cast<VkCullModeFlags>(builder.m_culling),
		static_cast<VkFrontFace>(builder.m_polyFrontFace),
		VK_FALSE,
		0.0f,
		0.0f,
		0.0f,
		1.0f
	};
	// multi-sample / anti aliasing
	VkPipelineMultisampleStateCreateInfo multisampling {
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		nullptr,
		0,
		static_cast<VkSampleCountFlagBits>((
			(renderer::globalRenderSystem->swapchain->colorImage.samples > builder.m_sampleCount) || 
			(builder.m_sampleCount == Image::SampleCount::bit0)
		) ? 
			renderer::globalRenderSystem->swapchain->colorImage.samples : 
			builder.m_sampleCount),
		VK_FALSE,
		0.0f,
		nullptr,
		VK_FALSE,
		VK_FALSE
	};

	// configure sample shading
	if (std::holds_alternative<float32>(builder.m_sampleShading)) {
		multisampling.sampleShadingEnable = VK_TRUE;
		multisampling.minSampleShading = std::get<float32>(builder.m_sampleShading);
	}

	// depth stencil / depth buffering
	VkPipelineDepthStencilStateCreateInfo depthStencilState {
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,
		VK_FALSE,
		VK_COMPARE_OP_NEVER,
		VK_FALSE,
		VK_FALSE,
		{ },
		{ },
		0.0f,
		0.0f
	};

	// configure depth stencil writing and comparison
	if (std::holds_alternative<Builder::DepthStencil>(builder.m_depthStencil)) {
		const auto& s = std::get<Builder::DepthStencil>(builder.m_depthStencil);

		depthStencilState.depthTestEnable = VK_TRUE;
		depthStencilState.depthWriteEnable = (s.write ? VK_TRUE : VK_FALSE);
		depthStencilState.depthCompareOp = static_cast<VkCompareOp>(s.compare);
	}

	// color blending
	VkPipelineColorBlendStateCreateInfo colorBlending {
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,
		VK_LOGIC_OP_COPY,
		static_cast<uint32>(builder.m_blendAttachments.size()),
		builder.m_blendAttachments.data(),
		{
			0.0f, 0.0f, 0.0f, 0.0f
		}
	};

	// viewport state
	VkPipelineViewportStateCreateInfo viewportState {
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		nullptr,
		0,
		1,
		nullptr,
		1,
		nullptr
	};

	// configure dynamic state
	Vector<VkDynamicState> dynamicState;
	// viewport
	if (std::holds_alternative<bool>(builder.m_viewport)) {
		dynamicState.pushBack(VK_DYNAMIC_STATE_VIEWPORT);
		dynamicViewport = VkViewport();
	} else {
		viewportState.viewportCount = 1;
		viewportState.pViewports = &std::get<VkViewport>(builder.m_viewport);
	} 
	// scissor
	if (std::holds_alternative<bool>(builder.m_scissor)) {
		dynamicState.pushBack(VK_DYNAMIC_STATE_SCISSOR);
		dynamicScissor = VkRect2D();
	} else {
		viewportState.scissorCount = 1;
		viewportState.pScissors = &std::get<VkRect2D>(builder.m_scissor);
	} 

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo {
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32>(dynamicState.size()),
		dynamicState.data()
	};

	Array<VkPipelineShaderStageCreateInfo, 2> tmpShaders = { program->vertexShader->stageCreateInfo(), program->fragmentShader->stageCreateInfo() };

	VkGraphicsPipelineCreateInfo createInfo {
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,	
		nullptr,
		0,
		2,
		tmpShaders.data(),
		&vertexInputInfo,
		&inputAssembly,
		nullptr,
		&viewportState,
		&rasterizer,
		&multisampling,
		&depthStencilState,
		&colorBlending,
		&dynamicStateCreateInfo,
		program->pipelineLayout,
		renderTarget->renderPass,
		0,
		VK_NULL_HANDLE,
		0
	};

	pipeline = vk::GraphicsPipeline(renderer::globalRenderSystem->device, renderer::globalRenderSystem->pipelineCache, createInfo);
}

void GraphicsPipeline::bind() const {
	renderer::globalRenderSystem->commandQueue->activeCommandBuffer->bindPipeline(static_cast<VkPipelineBindPoint>(bindPoint), pipeline);
	renderer::globalRenderSystem->commandQueue->pipelineStageFlags.pushBack(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

	if (std::holds_alternative<VkViewport>(dynamicViewport)) {
		renderer::globalRenderSystem->commandQueue->activeCommandBuffer->setViewport(std::get<VkViewport>(dynamicViewport));
	}

	if (std::holds_alternative<VkRect2D>(dynamicScissor)) {
		renderer::globalRenderSystem->commandQueue->activeCommandBuffer->setScissor(std::get<VkRect2D>(dynamicScissor));
	}
}

ImGuiRenderer::ImGuiRenderer() {
	Vector<const Image*> swapchainImages;
	swapchainImages.reserve(renderer::globalRenderSystem->swapchain->images.size());

	for (const auto& image : renderer::globalRenderSystem->swapchain->images)
		swapchainImages.pushBack(&image);

	m_renderTarget = RenderTarget({
		{
			{ &renderer::globalRenderSystem->swapchain->colorImage },
			RenderTarget::Attachment::Type::color,
			Image::Layout::colorAttachment,
			Image::Layout::undefined,
			Image::Layout::colorAttachment,
			Image::Aspect::color,
			0,
			{ 
				GPUMemory::LoadMode::clear,
				GPUMemory::StoreMode::store,
				GPUMemory::LoadMode::dontCare,
				GPUMemory::StoreMode::dontCare
			},
			{
				true,
				VK_SUBPASS_EXTERNAL,
				Pipeline::Stage::colorAttachmentOutput,
				Pipeline::Stage::colorAttachmentOutput,
				GPUMemory::Access::none,
				GPUMemory::Access::colorAttachmentWrite
			}
		},
		{
			swapchainImages,
			RenderTarget::Attachment::Type::render,
			Image::Layout::colorAttachment,
			Image::Layout::undefined,
			Image::Layout::present,
			Image::Aspect::color,
			0,
			{ 
				GPUMemory::LoadMode::dontCare,
				GPUMemory::StoreMode::store,
				GPUMemory::LoadMode::dontCare,
				GPUMemory::StoreMode::dontCare
			}
		}
	});

	Vector<vulkan::DescriptorPools::Size> poolSizes ({
		{ vulkan::DescriptorSets::Type::sampler, 512 },
		{ vulkan::DescriptorSets::Type::imageSampler, 512 },
		{ vulkan::DescriptorSets::Type::sampledImage, 512 },
		{ vulkan::DescriptorSets::Type::storageImage, 512 },
		{ vulkan::DescriptorSets::Type::texelBuffer, 512 },
		{ vulkan::DescriptorSets::Type::texelStorageBuffer, 512 },
		{ vulkan::DescriptorSets::Type::uniformBuffer, 512 },
		{ vulkan::DescriptorSets::Type::storageBuffer, 512 },
		{ vulkan::DescriptorSets::Type::dynamicUniformBuffer, 512 },
		{ vulkan::DescriptorSets::Type::dynamicStorageBuffer, 512 },
		{ vulkan::DescriptorSets::Type::inputAttachment, 512 }
	});

	m_descriptorPools = vulkan::DescriptorPools(poolSizes);

	ImGui_ImplSDL3_InitForVulkan(renderer::globalWindow->window);
	ImGui_ImplVulkan_InitInfo initInfo{
		renderer::globalRenderSystem->instance,
		renderer::globalRenderSystem->physicalDevice,
		renderer::globalRenderSystem->device,
		renderer::globalRenderSystem->queueFamilies.graphicsFamilyIndex,
		renderer::globalRenderSystem->graphicsQueue,
		m_descriptorPools.descriptorPools[0],
		//m_renderTarget.renderPass,
		renderer::globalRenderSystem->defaultRenderTarget->renderPass,
		2,
		static_cast<uint32>(renderer::globalRenderSystem->swapchain->images.size()),
		static_cast<VkSampleCountFlagBits>(renderer::globalRenderSystem->swapchain->colorImage.samples),
		//VK_SAMPLE_COUNT_1_BIT,
		renderer::globalRenderSystem->pipelineCache,
		0,
		VK_FALSE,
#ifdef VK_KHR_dynamic_rendering
		{ },
#endif
		nullptr,
		&imguiVulkanAssert
	};
	ImGui_ImplVulkan_Init(&initInfo);
}

void ImGuiRenderer::uploadFonts() {
	ImGui_ImplVulkan_CreateFontsTexture();
}

ImGuiRenderer::~ImGuiRenderer() {
	ImGui_ImplVulkan_DestroyFontsTexture();
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL3_Shutdown();
}

void ImGuiRenderer::beginFrame() {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void ImGuiRenderer::endFrame() {
	ImGui::Render();
	
	if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	//m_renderTarget.begin();
	renderer::globalRenderSystem->defaultRenderTarget->begin();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), renderer::globalRenderSystem->commandQueue->activeCommandBuffer->commandBuffer);
	renderer::globalRenderSystem->defaultRenderTarget->end();
	//m_renderTarget.end();
}

} // namespace vulkan

} // namespace lyra
