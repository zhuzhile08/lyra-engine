#include <Graphics/VulkanRenderSystem.h>

#include <Common/Logger.h>
#include <Common/Settings.h>

#include <SDL_vulkan.h>

#include <utility>
#include <limits>
#include <map>
#include <unordered_set>

namespace lyra {

namespace vulkan {

/**
 * @brief global class for the rendering systems of the engine
 */
struct RenderSystem {
	struct QueueFamilies {
		uint32 graphicsFamilyIndex = std::numeric_limits<uint32>::max();
		uint32 computeFamilyIndex = std::numeric_limits<uint32>::max();
		uint32 copyFamilyIndex = std::numeric_limits<uint32>::max();
		// uint32 videoFamilyIndex = VK_QUEUE_FAMILY_IGNORED; @todo
	};

	RenderSystem(const InitInfo& info, bool& sucess) {
		{ // create instance
			// check if requested validation layers are available
#ifndef NDEBUG
			uint32 availableValidationLayerCount = 0;
			vkEnumerateInstanceLayerProperties(&availableValidationLayerCount, nullptr);
			std::vector <VkLayerProperties> availableValidationLayers(availableValidationLayerCount);
			vkEnumerateInstanceLayerProperties(&availableValidationLayerCount, availableValidationLayers.data());

			// go through every requested layers and see if they are available
			for (const auto& requestedValidationLayer : info.requestedValidationLayers) {
				bool found = false;
				log().info("Available layers:");

				for (const auto& availableValidationLayer : availableValidationLayers) {
					log().debug(log().tab(), availableValidationLayer.layerName, ": ", availableValidationLayer.description);
					if (strcmp(requestedValidationLayer, availableValidationLayer.layerName) == 0) {
						found = true;
						break;
					}
				}

				lassert(found, "User required Vulkan validation layer wasn't found: ", requestedValidationLayer);
			}
#endif
			// get all extensions
			uint32 SDLExtensionCount = 0;
			lassert(SDL_Vulkan_GetInstanceExtensions(info.window, &SDLExtensionCount, nullptr) == SDL_TRUE, "Failed to get number of Vulkan instance extensions");
			std::vector<const char*> SDLExtensions(SDLExtensionCount);
			lassert(SDL_Vulkan_GetInstanceExtensions(info.window, &SDLExtensionCount, SDLExtensions.data()) == SDL_TRUE, "Failed to get Vulkan instance extensions");
			// add some required extensions
			SDLExtensions.push_back("VK_KHR_get_physical_device_properties2");
#ifdef __APPLE__
			SDLExtensions.push_back("VK_KHR_portability_enumeration");
#endif

			// define some info for the application that will be used in instance creation
			VkApplicationInfo appInfo{
				VK_STRUCTURE_TYPE_APPLICATION_INFO,
				nullptr,
				info.title.c_str(),
				VK_MAKE_API_VERSION(0, info.version[0], info.version[1], info.version[2]),
				"LyraEngine",
				VK_MAKE_API_VERSION(0, 0, 7, 0),
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
				static_cast<uint32>(info.requestedValidationLayers.size()),
				info.requestedValidationLayers.data(),
#else
				0,
				nullptr,
#endif
				static_cast<uint32>(SDLExtensions.size()),
				SDLExtensions.data()
			};

			// create the instance
			instance = vk::Instance(VK_NULL_HANDLE, createInfo);
		}

		{ // create the surface
			surface = vk::Surface(instance, info.window);
		}
		
		{ // find a suitable physical device
			// structure to hold important data to retrieve from a physical device
			struct PhysicalDeviceData {
				VkPhysicalDevice device;
				VkPhysicalDeviceProperties properties;
				VkPhysicalDeviceFeatures features;
				QueueFamilies queueFamilies;
			};

			// get all devices
			uint32 deviceCount = 0;
			vassert(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr), "find any Vulkan suitable GPUs");
			std::vector <VkPhysicalDevice> devices(deviceCount);			 // just put this in here cuz I was lazy
			vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

			// a ordered map with every GPU. The one with the highest score is the one that is going to be the used GPU
			std::multimap <uint32, PhysicalDeviceData> possibleDevices;

			for (const auto& device : devices) {
				log().info("GPU ", std::to_string((&device - &devices[0]) + 1), ": ");
				{ // rate the physical device
					// set the score to 1 first, if the GPU does not have required features, set it to 0
					int score = 0;

					// get the queue families
					QueueFamilies localQueueFamilies;

					uint32 queueFamilyCount = 0;
					vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
					std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
					vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

					uint32 i = 0;
					for (const auto& property : queueFamilyProperties) {
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
							localQueueFamilies.computeFamilyIndex != std::numeric_limits<uint32>::max()
						) {
							score = 1;
							break;
						}

						i++;
					}

					if (score == 0) {
						log().warning("GPU does not have the required queue families available!");
					}
					
					// get the available extensions
					uint32 availableDeviceExtensionCount = 0;
					vkEnumerateDeviceExtensionProperties(device, nullptr, &availableDeviceExtensionCount, nullptr);
					std::vector <VkExtensionProperties> availableDeviceExtensions(availableDeviceExtensionCount);
					vkEnumerateDeviceExtensionProperties(device, nullptr, &availableDeviceExtensionCount, availableDeviceExtensions.data());	

					// get some device properties and features
					VkPhysicalDeviceProperties properties;
					vkGetPhysicalDeviceProperties(device, &properties);
					VkPhysicalDeviceFeatures features;
					vkGetPhysicalDeviceFeatures(device, &features);

					// some required features. If not available, make the GPU unavailable
					if (
						!features.multiDrawIndirect || 
						[&]() -> bool {
	#ifndef NDEBUG
							// print all all availabe extensions
							log().info("Available device extensions:");
							for (const auto& availableDeviceExtension : availableDeviceExtensions) {
								log().debug(log().tab(), availableDeviceExtension.extensionName);
							}
	#endif
							// go through every requested extensions and see if they are available
							for (const auto& requestedDeviceExtension : info.requestedDeviceExtensions) {
								for (const auto& availableDeviceExtension : availableDeviceExtensions) {
									if (strcmp(requestedDeviceExtension, availableDeviceExtension.extensionName) == 0) {
										break;
									} else {
										return false;
									}
								}
							}

							return true;
						} ()
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
						score += (int)(properties.limits.maxImageDimension2D / 2048);

						log().info("Score: ", score, log().end_l());
					}

					// insert the device into the queue
					possibleDevices.emplace(score, PhysicalDeviceData {device, properties, features, localQueueFamilies});
				}
			}

			if (possibleDevices.rbegin()->first <= 0) {
				log().exception("Failed to find GPU with enough features");
			}

			physicalDevice = std::move(vk::PhysicalDevice(possibleDevices.rbegin()->second.device, instance));
			deviceProperties = possibleDevices.rbegin()->second.properties;
			deviceFeatures = possibleDevices.rbegin()->second.features;
			queueFamilies = possibleDevices.rbegin()->second.queueFamilies;
		}

		{ // create logical device
			std::vector <VkDeviceQueueCreateInfo> queueCreateInfos;
			std::unordered_set <uint32> uniqueQueueFamilies { queueFamilies.graphicsFamilyIndex, queueFamilies.computeFamilyIndex, queueFamilies.copyFamilyIndex };
			
			float queuePriority = 1.0f;
			for (const auto& queueFamily : uniqueQueueFamilies) {
				queueCreateInfos.push_back({
					VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
					nullptr,
					0,
					queueFamily,
					1,
					&queuePriority
				});
			}

			// device creation info
			VkDeviceCreateInfo createInfo {
				VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
				nullptr,
				0,
				static_cast<uint32>(queueCreateInfos.size()),
				queueCreateInfos.data(),
	#ifndef NDEBUG
				static_cast<uint32>(info.requestedValidationLayers.size()),
				info.requestedValidationLayers.data(),
	#else
				0,
				nullptr,
	#endif
				static_cast<uint32>(info.requestedDeviceExtensions.size()),
				info.requestedDeviceExtensions.data(),
				&deviceFeatures
			};
			// create the device
			device = vk::Device(physicalDevice, createInfo);
		}

		{ // create queues
			graphicsQueue = vk::Queue(device, queueFamilies.graphicsFamilyIndex, 0);
			computeQueue  = vk::Queue(device, queueFamilies.computeFamilyIndex, 0);
			copyQueue  = vk::Queue(device, queueFamilies.computeFamilyIndex, 0);
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
				VK_API_VERSION_1_2
			};

			// create the allocator
			allocator = vma::Allocator(instance, createInfo);
		}
	}

	/**
	 * @brief wrappers around the core Vulkan API and VMA functions
	 * @brief these are basically copied directly from the Vulkan API with minor modifications to reduce bloat and suit a more modern C++ style
	 * @brief please refer to the official Vulkan documentation (preferably at https:/devdocs.io// and https:/gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/) for the documentation of these functions
	 */

	VkResult findMemoryTypeIndexForBufferInfo(const VkBufferCreateInfo& bufferCreateInfo, const VmaAllocationCreateInfo& allocationCreateInfo, uint32 memoryTypeIndex) {
		return vmaFindMemoryTypeIndexForBufferInfo(allocator, &bufferCreateInfo, &allocationCreateInfo, &memoryTypeIndex);
	}
	VkResult findMemoryTypeIndexForImageInfo(const VkImageCreateInfo& pImageCreateInfo, const VmaAllocationCreateInfo& allocationCreateInfo, uint32 memoryTypeIndex) {
		return vmaFindMemoryTypeIndexForImageInfo(allocator, &pImageCreateInfo, &allocationCreateInfo, &memoryTypeIndex);
	}
	VkResult findMemoryTypeIndex(uint32 memoryTypeBits, const VmaAllocationCreateInfo& allocationCreateInfo, uint32 pMemoryTypeIndex) {
		return vmaFindMemoryTypeIndex(allocator, memoryTypeBits, &allocationCreateInfo, &pMemoryTypeIndex);
	}
	VkResult checkPoolCorruption(const VmaPool& pool) {
		return vmaCheckPoolCorruption(allocator, pool);
	}
	void getPoolName(const vma::Pool& pool, const char*& name) {
		vmaGetPoolName(allocator, pool, &name);
	}
	void setPoolName(const vma::Pool& pool, const char* name) {
		vmaSetPoolName(allocator, pool, name);
	}
	VkResult allocateMemoryPage(const VkMemoryRequirements& memoryRequirements, const VmaAllocationCreateInfo& createInfo, vma::Allocation& allocation, VmaAllocationInfo& allocationInfo) {
		return vmaAllocateMemoryPages(allocator, &memoryRequirements, &createInfo, 1, &allocation.get(), &allocationInfo);
	}
	VkResult allocateMemoryPages(const VkMemoryRequirements& memoryRequirements, const VmaAllocationCreateInfo& createInfo, std::vector<VmaAllocation>& allocations, VmaAllocationInfo& allocationInfo) {
		return vmaAllocateMemoryPages(allocator, &memoryRequirements, &createInfo, allocations.size(), allocations.data(), &allocationInfo);
	}
	VkResult allocateMemoryForBuffer(const vk::Buffer& buffer, const VmaAllocationCreateInfo& createInfo, vma::Allocation& allocation, VmaAllocationInfo& allocationInfo) {
		return vmaAllocateMemoryForBuffer(allocator, buffer, &createInfo, &allocation.get(), &allocationInfo);
	}
	VkResult allocateMemoryForImage(const vk::Image& image, const VmaAllocationCreateInfo& createInfo, vma::Allocation& allocation, VmaAllocationInfo& allocationInfo) {
		return vmaAllocateMemoryForImage(allocator, image, &createInfo, &allocation.get(), &allocationInfo);
	}
	void freeMemoryPage(const vma::Allocation& allocation) {
		vmaFreeMemoryPages(allocator, 1, &allocation.get());
	}
	void freeMemoryPages(const std::vector<VmaAllocation>& allocations) {
		vmaFreeMemoryPages(allocator, allocations.size(), allocations.data());
	}
	void getAllocationInfo(const vma::Allocation& allocation, VmaAllocationInfo& pAllocationInfo) {
		vmaGetAllocationInfo(allocator, allocation, &pAllocationInfo);
	}
	void setAllocationUserData(const vma::Allocation& allocation, void* pUserData) {
		vmaSetAllocationUserData(allocator, allocation, pUserData);
	}
	void setAllocationName(const vma::Allocation& allocation, const char* pName) {
		vmaSetAllocationName(allocator, allocation, pName);
	}
	void getAllocationMemoryProperties(const vma::Allocation& allocation, VkMemoryPropertyFlags& flags) {
		vmaGetAllocationMemoryProperties(allocator, allocation, &flags);
	}
	VkResult flushAllocation(const vma::Allocation& allocation, VkDeviceSize offset, VkDeviceSize size) {
		return vmaFlushAllocation(allocator, allocation, offset, size);
	}
	VkResult flushAllocations(const std::vector<VmaAllocation>& allocations, const std::vector<VkDeviceSize> offsets, const std::vector<VkDeviceSize> sizes) {
		return vmaFlushAllocations(allocator, allocations.size(), allocations.data(), offsets.data(), sizes.data());
	}
	VkResult invalidateAllocation(const vma::Allocation& allocation, VkDeviceSize offset, VkDeviceSize size) {
		return vmaInvalidateAllocation(allocator, allocation.get(), offset, size);
	}
	VkResult invalidateAllocations(const std::vector<VmaAllocation>& allocations, const std::vector<VkDeviceSize> offsets, const std::vector<VkDeviceSize> sizes) {
		return vmaInvalidateAllocations(allocator, allocations.size(), allocations.data(), offsets.data(), sizes.data());
	}
	VkResult checkCorruption(uint32 memoryTypeBits) {
		return vmaCheckCorruption(allocator, memoryTypeBits);
	}
	VkResult beginDefragmentation(const VmaDefragmentationInfo& info, vma::DefragmentationContext& context) {
		return vmaBeginDefragmentation(allocator, &info, &context.get());
	}
	void endDefragmentation(const vma::DefragmentationContext& context, VmaDefragmentationStats& pStats) {
		vmaEndDefragmentation(allocator, context, &pStats);
	}
	VkResult beginDefragmentationPass(const vma::DefragmentationContext& context, VmaDefragmentationPassMoveInfo& passInfo) {
		return vmaBeginDefragmentationPass(allocator, context, &passInfo);
	}
	VkResult endDefragmentationPass(const vma::DefragmentationContext& context, VmaDefragmentationPassMoveInfo& passInfo) {
		return vmaEndDefragmentationPass(allocator, context, &passInfo);
	}
	VkResult bindBufferMemory(const vma::Allocation& allocation, const vk::Buffer& buffer) {
		return vmaBindBufferMemory(allocator, allocation, buffer);
	}
	VkResult bindBufferMemory2(const vma::Allocation& allocation, VkDeviceSize allocationLocalOffset, const vk::Buffer& buffer, const void* pNext) {
		return vmaBindBufferMemory2(allocator, allocation, allocationLocalOffset, buffer, pNext);
	}
	VkResult bindImageMemory(const vma::Allocation& allocation, const vk::Image& image) {
		return vmaBindImageMemory(allocator, allocation, image);
	}
	VkResult bindImageMemory2(const vma::Allocation& allocation, VkDeviceSize allocationLocalOffset, const vk::Image& image, const void* pNext) {
		return vmaBindImageMemory2(allocator, allocation, allocationLocalOffset, image, pNext);
	}
	void freeMemory(const vma::Allocation& allocation) {
		vmaFreeMemory(allocator, allocation);
	}
	VkResult flushMappedMemoryRange(const VkMappedMemoryRange& memoryRange) {
		return vkFlushMappedMemoryRanges(device, 1, &memoryRange);
	}
	VkResult flushMappedMemoryRanges(const std::vector<VkMappedMemoryRange>& memoryRanges) {
		return vkFlushMappedMemoryRanges(device, memoryRanges.size(), memoryRanges.data());
	}
	VkResult getEventStatus(const vk::Event& event) {
		return vkGetEventStatus(device, event);
	}
	VkResult getFenceStatus(const vk::Fence& fence) {
		return vkGetFenceStatus(device, fence);
	}
	void getImageMemoryRequirements(const VkImage& image, VkMemoryRequirements& memoryRequirements) {
		vkGetImageMemoryRequirements(device, image, &memoryRequirements);
	}
	void getImageSparseMemoryRequirements(const vk::Image& image, std::vector<VkSparseImageMemoryRequirements>& sparseMemoryRequirements) {
		uint32 s;
		vkGetImageSparseMemoryRequirements(device, image, &s, nullptr);
		sparseMemoryRequirements.resize(s);
		vkGetImageSparseMemoryRequirements(device, image, &s, sparseMemoryRequirements.data());
	}
	void getImageSubresourceLayout(const vk::Image& image, const VkImageSubresource& subresource, VkSubresourceLayout& layout) {
		vkGetImageSubresourceLayout(device, image, &subresource, &layout);
	}
	VkResult getPipelineCacheData(const vk::PipelineCache& pipelineCache, size_t& pDataSize, void* pData) {
		return vkGetPipelineCacheData(device, pipelineCache, &pDataSize, pData);
	}
	VkResult getQueryPoolResults(const vk::QueryPool& queryPool, uint32 firstQuery, uint32 queryCount, size_t dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags) {
		return vkGetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
	}
	void getRenderAreaGranularity(const vk::RenderPass& renderPass, VkExtent2D& pGranularity) {
		vkGetRenderAreaGranularity(device, renderPass, &pGranularity);
	}
	VkResult invalidateMappedMemoryRange(const VkMappedMemoryRange& memoryRange) {
		return vkInvalidateMappedMemoryRanges(device, 1, &memoryRange);
	}
	VkResult invalidateMappedMemoryRanges(const std::vector<VkMappedMemoryRange>& memoryRanges) {
		return vkInvalidateMappedMemoryRanges(device, memoryRanges.size(), memoryRanges.data());
	}
	VkResult mapMemory(const vma::Allocation& allocation, void** ppData) {
		return vmaMapMemory(allocator, allocation, ppData);
	}
	VkResult mergePipelineCache(const vk::PipelineCache& dstCache, const vk::PipelineCache srcCache) {
		return vkMergePipelineCaches(device, dstCache, 1, &srcCache.get());
	}
	VkResult mergePipelineCaches(const vk::PipelineCache& dstCache, const std::vector<VkPipelineCache>& srcCaches) {
		return vkMergePipelineCaches(device, dstCache, srcCaches.size(), srcCaches.data());
	}
	VkResult resetCommandPool(const vk::CommandPool& commandPool, VkCommandPoolResetFlags flags) {
		return vkResetCommandPool(device, commandPool, flags);
	}
	VkResult resetDescriptorPool(const vk::DescriptorPool& descriptorPool, VkDescriptorPoolResetFlags flags) {
		return vkResetDescriptorPool(device, descriptorPool, flags);
	}
	VkResult resetEvent(const vk::Event& event) {
		return vkResetEvent(device, event);
	}
	VkResult resetFence(const vk::Fence& fence) {
		return vkResetFences(device, 1, &fence.get());
	}
	VkResult resetFences(uint32 fenceCount, const VkFence* fences) {
		return vkResetFences(device, fenceCount, fences);
	}
	VkResult setEvent(const vk::Event& event) {
		return vkSetEvent(device, event.get());
	}
	void unmapMemory(const vma::Allocation& allocation) {
		vmaUnmapMemory(allocator.get(), allocation);
	}
	void updateDescriptorSet(const VkWriteDescriptorSet& descriptorWrite, const VkCopyDescriptorSet& descriptorCopy) {
		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 1, &descriptorCopy);
	}
	void updateDescriptorSets(const std::vector<VkWriteDescriptorSet>& descriptorWrites) {
		vkUpdateDescriptorSets(device, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
	void updateDescriptorSets(const std::vector<VkWriteDescriptorSet>& descriptorWrites, const std::vector<VkCopyDescriptorSet>& descriptorCopies) {
		vkUpdateDescriptorSets(device, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), static_cast<uint32>(descriptorCopies.size()), descriptorCopies.data());
	}
	VkResult waitForFence(const vk::Fence& fence, VkBool32 waitAll, uint64 timeout) {
		return vkWaitForFences(device, 1, &fence.get(), waitAll, timeout);
	}
	VkResult waitForFences(const std::vector<VkFence>& fences, VkBool32 waitAll, uint64 timeout) {
		return vkWaitForFences(device, static_cast<uint32>(fences.size()), fences.data(), waitAll, timeout);
	}

	vk::Instance instance;
	vk::Surface surface;

	vk::PhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vk::Device device;

	QueueFamilies queueFamilies;
	vk::Queue graphicsQueue;
	vk::Queue computeQueue;
	vk::Queue copyQueue;

	vma::Allocator allocator;
};

static RenderSystem* globalRenderSystem;

bool init_render_system(const InitInfo& info) {
	bool sucess;
	globalRenderSystem = new RenderSystem(info, sucess);
	return sucess;
}

CommandQueue::CommandPool::CommandPool() {
	VkCommandPoolCreateInfo createInfo{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		globalRenderSystem->queueFamilies.graphicsFamilyIndex
	};

	commandPool = vk::CommandPool(globalRenderSystem->device, createInfo);
}

void CommandQueue::CommandPool::reset() {
	vassert(globalRenderSystem->resetCommandPool(commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT), "reset command pool");
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
	vassert(vkAllocateCommandBuffers(globalRenderSystem->device.get(), &allocInfo, &commandBuffer.get()), "create Vulkan command buffer");
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
	vassert(vkBeginCommandBuffer(commandBuffer, &beginInfo), "start recording Vulkan command buffer");
}

void CommandQueue::CommandBuffer::reset(VkCommandBufferResetFlags flags) const {
	vassert(vkResetCommandBuffer(commandBuffer, flags), "reset command buffer"); // reset the command buffer
}

CommandQueue::CommandQueue() { 
	commandPools.resize(Settings::RenderConfig::maxFramesInFlight);
}

void CommandQueue::reset() {
	auto newCommandQueue = CommandQueue();
	std::swap(*this, newCommandQueue);
}

void CommandQueue::submit(VkFence fence, bool wait) {
	if (activeCommandBuffer != VK_NULL_HANDLE) {
		VkSubmitInfo submitInfo {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			static_cast<uint32>(waitSemaphores.size()),
			waitSemaphores.data(),
			pipelineStageFlags.data(),
			1,
			&activeCommandBuffer,
			static_cast<uint32>(signalSemaphores.size()),
			signalSemaphores.data()
		};

		vassert(vkQueueSubmit(queue, 1, &submitInfo, fence), "submit Vulkan queue");

		if (wait) {
			vassert(globalRenderSystem->waitForFence(WeakRAIIContainer<VkFence>(fence, nullptr), VK_TRUE, std::numeric_limits<uint32>::max()), "wait for fence to finish");
		}

		activeCommandBuffer = VK_NULL_HANDLE;

		currentFrame = (currentFrame + 1) % Settings::RenderConfig::maxFramesInFlight;
	}

	waitSemaphores.clear();
	signalSemaphores.clear();
	pipelineStageFlags.clear();
}

} // namespace vulkan

} // namespace lyra
