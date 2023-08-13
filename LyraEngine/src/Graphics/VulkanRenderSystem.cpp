#include <Graphics/VulkanRenderSystem.h>

#include <Common/Utility.h>
#include <Common/Logger.h>
#include <Common/Config.h>

#include <SDL_vulkan.h>

#include <utility>
#include <limits>
#include <map>
#include <unordered_set>

namespace {

VKAPI_ATTR VkBool32 VKAPI_CALL validationCallBack(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData
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

}

namespace lyra {

namespace vulkan {

/**
 * @brief global class for the rendering systems of the engine
 */
class RenderSystem {
public:
	struct QueueFamilies {
		uint32 graphicsFamilyIndex = std::numeric_limits<uint32>::max();
		uint32 computeFamilyIndex = std::numeric_limits<uint32>::max();
		uint32 copyFamilyIndex = std::numeric_limits<uint32>::max();
		// uint32 videoFamilyIndex = VK_QUEUE_FAMILY_IGNORED; @todo
		
		std::vector <VkQueueFamilyProperties> queueFamilyProperties;
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
			uint32 instanceExtensionCount = 0;
			ASSERT(SDL_Vulkan_GetInstanceExtensions(info.window, &instanceExtensionCount, nullptr) == SDL_TRUE, "Failed to get number of Vulkan instance extensions");
			std::vector<const char*> instanceExtensions(instanceExtensionCount);
			ASSERT(SDL_Vulkan_GetInstanceExtensions(info.window, &instanceExtensionCount, instanceExtensions.data()) == SDL_TRUE, "Failed to get Vulkan instance extensions");
			// add some required extensions
			instanceExtensions.push_back("VK_KHR_get_physical_device_properties2");
#ifndef NDEBUG
			instanceExtensions.push_back("VK_EXT_debug_utils");
#endif
#ifdef __APPLE__
			instanceExtensions.push_back("VK_KHR_portability_enumeration");
#endif

#ifndef NDEBUG // debug messenger create info
			VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo {
				VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
				nullptr,
				0,
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
				validationCallBack
			};
#endif

			// define some info for the application that will be used in instance creation
			VkApplicationInfo appInfo{
				VK_STRUCTURE_TYPE_APPLICATION_INFO,
				nullptr,
				config::title.data(),
				VK_MAKE_API_VERSION(0, info.version[0], info.version[1], info.version[2]),
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

		{ // get some extension functions
			vkCmdPushDescriptorSetKHR = (PFN_vkCmdPushDescriptorSetKHR) vkGetInstanceProcAddr(instance, "vkCmdPushDescriptorSetKHR");
			vkCmdPushDescriptorSetWithTemplateKHR = (PFN_vkCmdPushDescriptorSetWithTemplateKHR) vkGetInstanceProcAddr(instance, "vkCmdPushDescriptorSetWithTempalteKHR");
		}
		
		{ // find a suitable physical device
			// structure to hold important data to retrieve from a physical device
			struct PhysicalDeviceData {
				VkPhysicalDevice device;
				VkPhysicalDeviceProperties properties;
				VkPhysicalDevicePushDescriptorPropertiesKHR pushDescriptorProperties;
				VkPhysicalDeviceFeatures features;
				QueueFamilies queueFamilies;
			};

			// get the extended physical device properties function
			auto vkGetPhysicalDeviceProperties2KHR = (PFN_vkGetPhysicalDeviceProperties2KHR) vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2KHR");
			ASSERT(vkGetPhysicalDeviceProperties2KHR, "vkGetInstanceProcAddr() for \"vkGetPhysicalDeviceProperties2KHR\" returned a null pointer!");

			// get all devices
			uint32 deviceCount = 0;
			VULKAN_ASSERT(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr), "find any Vulkan suitable GPUs");
			std::vector <VkPhysicalDevice> devices(deviceCount);			 // just put this in here cuz I was lazy
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
					std::vector <VkExtensionProperties> availableDeviceExtensions(availableDeviceExtensionCount);
					vkEnumerateDeviceExtensionProperties(device, nullptr, &availableDeviceExtensionCount, availableDeviceExtensions.data());	

					// get some device properties and features
					VkPhysicalDevicePushDescriptorPropertiesKHR pushDescriptorProperties {
						VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR
					};
					VkPhysicalDeviceProperties2KHR extendedProperties {
						VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR,
						&pushDescriptorProperties
					};
					vkGetPhysicalDeviceProperties2KHR(device, &extendedProperties);
					VkPhysicalDeviceFeatures features;
					vkGetPhysicalDeviceFeatures(device, &features);

					// some required features. If not available, make the GPU unavailable
					if (
						!features.multiDrawIndirect || 
						[&]() -> bool {
#ifndef NDEBUG
							// print all all availabe extensions
							log::info("Available device extensions:");
							for (const auto& availableDeviceExtension : availableDeviceExtensions) {
								log::debug("\t{}", availableDeviceExtension.extensionName);
							}
#endif
							std::unordered_set<std::string> requestedExtensions(config::requestedDeviceExtensions.begin(), config::requestedDeviceExtensions.end());
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

					log::info("Available device features and properties: ");

					// the actual scoring system
					if (score > 0) {
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
					possibleDevices.emplace(score, PhysicalDeviceData {device, extendedProperties.properties, pushDescriptorProperties, features, localQueueFamilies});
				}
			}

			if (possibleDevices.rbegin()->first <= 0) {
				ASSERT(false, "Failed to find GPU with enough features");
			} else log::info("Picked the GPU with a score of: {}!", possibleDevices.rbegin()->first);

			physicalDevice = std::move(vk::PhysicalDevice(possibleDevices.rbegin()->second.device, instance));
			deviceProperties = possibleDevices.rbegin()->second.properties;
			pushDescriptorProperties = possibleDevices.rbegin()->second.pushDescriptorProperties;
			deviceFeatures = possibleDevices.rbegin()->second.features;
			queueFamilies = possibleDevices.rbegin()->second.queueFamilies;
		}

		{ // create logical device
			std::vector <VkDeviceQueueCreateInfo> queueCreateInfos;
			std::unordered_set <uint32> uniqueQueueFamilies { queueFamilies.graphicsFamilyIndex, queueFamilies.computeFamilyIndex, queueFamilies.copyFamilyIndex };
			
			float32 queuePriority = 1.0f;
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
				static_cast<uint32>(config::requestedValidationLayers.size()),
				config::requestedValidationLayers.data(),
	#else
				0,
				nullptr,
	#endif
				static_cast<uint32>(config::requestedDeviceExtensions.size()),
				config::requestedDeviceExtensions.data(),
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
	vk::DebugUtilsMessenger debugMessenger;

	vk::PhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDevicePushDescriptorPropertiesKHR pushDescriptorProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vk::Device device;

	QueueFamilies queueFamilies;
	vk::Queue graphicsQueue;
	vk::Queue computeQueue;
	vk::Queue copyQueue;

	vma::Allocator allocator;

	PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR;
	PFN_vkCmdPushDescriptorSetWithTemplateKHR vkCmdPushDescriptorSetWithTemplateKHR;
};

static RenderSystem* globalRenderSystem;

bool init_render_system(const InitInfo& info) {
	bool sucess = true;
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
	VULKAN_ASSERT(globalRenderSystem->resetCommandPool(commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT), "reset command pool");
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
	VULKAN_ASSERT(vkAllocateCommandBuffers(globalRenderSystem->device.get(), &allocInfo, &commandBuffer.get()), "create Vulkan command buffer");
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

void CommandQueue::CommandBuffer::pushDescriptorSet(
	VkPipelineBindPoint bindPoint,
	const vk::PipelineLayout& layout,
	uint32 set,
	const VkWriteDescriptorSet& write) {
	globalRenderSystem->vkCmdPushDescriptorSetKHR(commandBuffer, bindPoint, layout, set, 1, &write);
}
void CommandQueue::CommandBuffer::pushDescriptorSet(
	VkPipelineBindPoint bindPoint,
	const vk::PipelineLayout& layout,
	uint32 set,
	const std::vector<VkWriteDescriptorSet>& writes) {
	globalRenderSystem->vkCmdPushDescriptorSetKHR(commandBuffer, bindPoint, layout, set, writes.size(), writes.data());
}
void CommandQueue::CommandBuffer::pushDescriptorSetWithTemplate(
	const vk::DescriptorUpdateTemplate& updateTemplate,
	const vk::PipelineLayout& layout,
	uint32 set,
	const void* data) {
	globalRenderSystem->vkCmdPushDescriptorSetWithTemplateKHR(commandBuffer, updateTemplate, layout, set, data);
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

		VULKAN_ASSERT(vkQueueSubmit(queue, 1, &submitInfo, fence), "submit Vulkan queue");

		if (wait) {
			VULKAN_ASSERT(globalRenderSystem->waitForFence(vk::Fence(fence, VK_NULL_HANDLE), VK_TRUE, std::numeric_limits<uint32>::max()), "wait for fence to finish");
		}

		activeCommandBuffer = VK_NULL_HANDLE;

		currentFrame = (currentFrame + 1) % config::maxFramesInFlight;
	}

	waitSemaphores.clear();
	signalSemaphores.clear();
	pipelineStageFlags.clear();
}

void CommandQueue::oneTimeBegin() {
	auto cmdBuff = CommandBuffer(commandPools.at(currentFrame));
	cmdBuff.begin(CommandQueue::CommandBuffer::Usage::USAGE_ONE_TIME_SUBMIT);
	activeCommandBuffer = cmdBuff.commandBuffer.release();
}

void CommandQueue::oneTimeSubmit() {
	auto cmdBuff = CommandBuffer(vk::CommandBuffer(activeCommandBuffer, globalRenderSystem->device));
	cmdBuff.end();
	submit(VK_NULL_HANDLE, false);
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
		globalRenderSystem->device, 
		globalRenderSystem->allocator, 
		createInfo, 
		get_alloc_create_info(memUsage), 
		memory
	);
}

void GPUBuffer::copy_data(const void* src, size_t copySize) {
	void* data;
	VULKAN_ASSERT(globalRenderSystem->mapMemory(memory, &data), "map buffer memory at {}", get_address(memory));
	
#ifndef NDEBUG
	memcpy(data, src, (copySize == 0) ? static_cast<size_t>(size) : copySize);
#else
	// custom memcpy functionality, (probably) faster in release mode
	const char* s = (char*)src;
	char* d = (char*)data;

	for (size_t i = 0; i < (copySize == 0) ? static_cast<size_t>(size) : copySize; i++) d[i] = s[i];

	data = std::move(d);
#endif

	globalRenderSystem->unmapMemory(memory);
}

void GPUBuffer::copy_data(const void** src, uint32 arraySize, size_t elementSize) {
	char* data;
	VULKAN_ASSERT(globalRenderSystem->mapMemory(memory, (void**)&data), "map buffer memory at {}", get_address(memory));

	for (uint32 i = 0; i < arraySize; i++) {
		memcpy(static_cast<void*>(data + elementSize * i), src[i], elementSize);
	}

	globalRenderSystem->unmapMemory(memory);
}

void GPUBuffer::copy(const GPUBuffer& srcBuffer) {
	CommandQueue commandQueue;
	commandQueue.oneTimeBegin();

	VkBufferCopy copyRegion{
		0,
		0,
		size
	};
	CommandQueue::CommandBuffer(commandQueue.activeCommandBuffer).copyBuffer(srcBuffer.buffer, buffer, copyRegion);

	commandQueue.oneTimeSubmit();
}

void Image::create_view(VkFormat format, const VkImageSubresourceRange& subresourceRange, VkImageViewType viewType, const VkComponentMapping& colorComponents) {
	VkImageViewCreateInfo createInfo{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0,
		image,
		viewType,
		format,
		colorComponents,
		subresourceRange
	};

	view = vk::ImageView(globalRenderSystem->device, createInfo);
}

void Image::transition_layout(
	VkImageLayout oldLayout,
	VkImageLayout newLayout,
	VkFormat format,
	const VkImageSubresourceRange& subresourceRange
) const {
	CommandQueue commandQueue;
	commandQueue.oneTimeBegin();

	// goofy ahh check to see which image layout to use
	VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	VkAccessFlags sourceAccess = 0;
	VkAccessFlags destinationAccess = VK_ACCESS_TRANSFER_WRITE_BIT;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		sourceAccess = 0; destinationAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		sourceAccess = VK_ACCESS_TRANSFER_WRITE_BIT; destinationAccess = VK_ACCESS_SHADER_READ_BIT;
		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT; destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		sourceAccess = 0; destinationAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else ASSERT(false, "Invalid image layout transition was requested whilst transitioning an image layout at: {}!", get_address(this));

	CommandQueue::CommandBuffer(commandQueue.activeCommandBuffer).pipelineBarrier(
		sourceStage, 
		destinationStage, 
		0,
		VkMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM },
		VkBufferMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM }, 
		get_image_memory_barrier(
			sourceAccess, 
			destinationAccess, 
			oldLayout, 
			newLayout, 
			subresourceRange
		)
	);

	commandQueue.oneTimeSubmit();
}

VkFormat Image::get_best_format(const std::vector<VkFormat>& candidates, VkFormatFeatureFlags features, VkImageTiling tiling) {
	for (const auto& candidate : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(globalRenderSystem->physicalDevice, candidate, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) return candidate;
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) return candidate;
	}

	ASSERT(false, "Failed to find supported format out of user defined formats!");

	return VK_FORMAT_MAX_ENUM;
}

void Image::copy_from_buffer(const vulkan::GPUBuffer& stagingBuffer, const VkExtent3D& extent, uint32 layerCount) {
	CommandQueue commandQueue;
	commandQueue.oneTimeBegin();

	VkBufferImageCopy imageCopy {
		0,
		0,
		0,
		{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, layerCount},
		{0, 0, 0},
		extent
	};
	CommandQueue::CommandBuffer(commandQueue.activeCommandBuffer).copyBufferToImage(stagingBuffer.buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageCopy);

	commandQueue.oneTimeSubmit();
}

Swapchain::Swapchain(SDL_Window* window, CommandQueue& commandQueue) : window(window), commandQueue(&commandQueue) {
	surface = vk::Surface(globalRenderSystem->instance, window);

	{ // check present queue compatibility
		uint32 familyIndex = 0;
		for (const auto& queueFamilyProperty : globalRenderSystem->queueFamilies.queueFamilyProperties) {
			VkBool32 presentSupport;
			VULKAN_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(globalRenderSystem->physicalDevice, familyIndex, surface, &presentSupport),
				"check physical device queue family presentation support");
			
			if (presentFamilyIndex == std::numeric_limits<uint32>::max() && presentSupport == VK_TRUE) {
				presentFamilyIndex = familyIndex;
				break;
			}

			familyIndex++;
		}

		if (presentFamilyIndex == std::numeric_limits<uint32>::max()) {
			ASSERT(false, "Failed to find queue family with presentation support with physical device: {} for surface: {}!", get_address(globalRenderSystem->physicalDevice), get_address(surface));
		}
	}

	{ // create syncronization objects
		VkSemaphoreCreateInfo semaphoreInfo{
			VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
		};

		VkFenceCreateInfo fenceInfo{
			VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			nullptr,
			VK_FENCE_CREATE_SIGNALED_BIT
		};

		for (uint32 i = 0; i < config::maxFramesInFlight; i++) {
			imageAquiredSemaphores[i] = vk::Semaphore(globalRenderSystem->device, semaphoreInfo);
			submitFinishedSemaphores[i] = vk::Semaphore(globalRenderSystem->device, semaphoreInfo);
			renderFinishedFences[i] = vk::Fence(globalRenderSystem->device, fenceInfo);
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
		vkGetPhysicalDeviceSurfaceFormatsKHR(globalRenderSystem->physicalDevice, surface, &availableFormatCount, nullptr);
		std::vector <VkSurfaceFormatKHR> availableFormats(availableFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(globalRenderSystem->physicalDevice, surface, &availableFormatCount, availableFormats.data());
		// check the formats
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				surfaceFormat = availableFormat;
			}
		}

		surfaceFormat = availableFormats[0];
		format = surfaceFormat.format;

		log::debug("\tFormat is: {} (preferred format is format: {} with color space: {})", format, VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
	}
	
	VkPresentModeKHR presentMode;
	{ // determine the presentation mode
		uint32 availablePresentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(globalRenderSystem->physicalDevice, surface, &availablePresentModeCount, nullptr);
		std::vector <VkPresentModeKHR> availablePresentModes(availablePresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(globalRenderSystem->physicalDevice, surface, &availablePresentModeCount, availablePresentModes.data());
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
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(globalRenderSystem->physicalDevice, surface, &surfaceCapabilities);

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
		int width, height;
		SDL_Vulkan_GetDrawableSize(window, &width, &height);

		VkExtent2D newExtent = {
			static_cast<uint32>(width),
			static_cast<uint32>(height)
		};

		newExtent.width = std::clamp(newExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		newExtent.height = std::clamp(newExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

		extent = newExtent;
	}

	{ // create swapchain
		VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		std::vector<uint32> queueFamilies;

		if (presentFamilyIndex != globalRenderSystem->queueFamilies.graphicsFamilyIndex) {
			sharingMode = VK_SHARING_MODE_CONCURRENT;
			queueFamilies = { globalRenderSystem->queueFamilies.graphicsFamilyIndex, presentFamilyIndex };
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
			(oldSwapchain != nullptr) ? *oldSwapchain : VK_NULL_HANDLE
		};

		swapchain = vk::Swapchain(globalRenderSystem->device, createInfo);
	}

	{ // create swapchain images
		std::vector<VkImage> tempImages;
		
		uint32 imageCount;
		VULKAN_ASSERT(vkGetSwapchainImagesKHR(globalRenderSystem->device, swapchain, &imageCount, nullptr), "retrieve Vulkan swapchain images");
		images.resize(imageCount); tempImages.resize(imageCount);
		vkGetSwapchainImagesKHR(globalRenderSystem->device, swapchain, &imageCount, tempImages.data());

		for (auto& image : images) {
			uint32 i = &image - &images[0];
			image.image = std::move(tempImages[i]);

			image.create_view(format, { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
		}
	}
}

void Swapchain::createAttachments() {
	{ // create anti aliasing images
		{ // configure the multisample
			VkSampleCountFlags counts = globalRenderSystem->deviceProperties.limits.framebufferColorSampleCounts & globalRenderSystem->deviceProperties.limits.framebufferDepthSampleCounts;

			if (counts & VK_SAMPLE_COUNT_64_BIT) { maxMultisamples = VK_SAMPLE_COUNT_64_BIT; }
			else if (counts & VK_SAMPLE_COUNT_32_BIT) { maxMultisamples = VK_SAMPLE_COUNT_32_BIT; }
			else if (counts & VK_SAMPLE_COUNT_16_BIT) { maxMultisamples = VK_SAMPLE_COUNT_16_BIT; }
			else if (counts & VK_SAMPLE_COUNT_8_BIT) { maxMultisamples = VK_SAMPLE_COUNT_8_BIT; }
			else if (counts & VK_SAMPLE_COUNT_4_BIT) { maxMultisamples = VK_SAMPLE_COUNT_4_BIT; }
			else if (counts & VK_SAMPLE_COUNT_2_BIT) { maxMultisamples = VK_SAMPLE_COUNT_2_BIT; }
			else { maxMultisamples = VK_SAMPLE_COUNT_1_BIT; }
		}

		colorImage.image = vk::Image(
			globalRenderSystem->device,
			globalRenderSystem->allocator, 
			colorImage.get_image_create_info(
				format,
				{ extent.width, extent.height, 1 },
				VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				1,
				VK_IMAGE_TYPE_2D,
				1,
				0,
				maxMultisamples
			),
			GPUMemory::get_alloc_create_info(VMA_MEMORY_USAGE_GPU_ONLY, VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)),
			colorMem.memory
		);

		colorImage.create_view(format, { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
	}
	
	{ // create depth images
		depthBufferFormat = Image::get_best_format({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_TILING_OPTIMAL);

		depthImage.image = vk::Image(
			globalRenderSystem->device,
			globalRenderSystem->allocator, 
			depthImage.get_image_create_info(
				depthBufferFormat,
				{ extent.width, extent.height, 1 },
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				1,
				VK_IMAGE_TYPE_2D,
				1,
				0,
				maxMultisamples
			),
			GPUMemory::get_alloc_create_info(VMA_MEMORY_USAGE_GPU_ONLY, VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)),
			depthMem.memory
		);

		depthImage.create_view(VK_FORMAT_D32_SFLOAT, { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 });
	}
}

void Swapchain::update(bool windowModified) {
	if (lostSurface) {
		surface.destroy();
		surface = vk::Surface(globalRenderSystem->instance, window);
		lostSurface = false;
	}

	if (invalidSwapchain || lostSurface || windowModified) {
		if (oldSwapchain != nullptr) vkDestroySwapchainKHR(globalRenderSystem->device, *oldSwapchain, nullptr);
		oldSwapchain = &(this->swapchain);

		createSwapchain();
		invalidSwapchain = false;
	}

	if (invalidAttachments || (invalidSwapchain || lostSurface || windowModified)) {
		for (auto& image : images) {
			image.destroy();
		}

		colorImage.destroy();
		colorMem.destroy();
		depthImage.destroy();
		depthMem.destroy();
		swapchain.destroy();

		createAttachments();
		invalidAttachments = false;
	}
}

void Swapchain::aquire() {
	VkResult result = vkAcquireNextImageKHR(
		globalRenderSystem->device, 
		swapchain, 
		std::numeric_limits<uint32>::max(), 
		imageAquiredSemaphores[commandQueue->currentFrame], 
		VK_NULL_HANDLE, 
		&commandQueue->currentFrame
	);

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
			VULKAN_ASSERT(result, "aquire next vulkan swapchain image");
			break;
	}
}

void Swapchain::present() {
	VkPresentInfoKHR presentInfo {
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1,
		&submitFinishedSemaphores[commandQueue->currentFrame].get(),
		1,
		&swapchain.get(),
		&commandQueue->currentFrame
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

	globalRenderSystem->waitForFence(
		renderFinishedFences[commandQueue->currentFrame], 
		VK_TRUE, 
		std::numeric_limits<uint64>::max()
	);
}

Program::Program(const Shader& vertexShader, const Shader& fragmentShader) {
	Array<std::vector<VkDescriptorSetLayoutBinding>, 3> bindings;
	descriptorSetLayouts.resize(3);

	bindings[0] = {{
		0,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		1,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		nullptr
	}};

	bindings[1] = {
		{
			0,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			1,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			nullptr
		},
		{ // albedo
			1,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			config::maxTexturesPerBinding,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			nullptr
		},
		{ // metallic
			2,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			config::maxTexturesPerBinding,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			nullptr
		},
		{ // emission
			3,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			config::maxTexturesPerBinding,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			nullptr
		},
		{ // occlusion map
			4,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			config::maxTexturesPerBinding,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			nullptr
		}
	};

	bindings[2] = {{
		0,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		1,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		nullptr
	}};

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		3,
		nullptr,
		0,
		nullptr
	};

	Array<VkDescriptorSetLayout, 3> tmpLayouts;

	for (uint32 i = 0; i < 3; i++) {
		VkDescriptorSetLayoutBindingFlagsCreateInfo bindingExt {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
		};

		VkDescriptorSetLayoutCreateInfo createInfo{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			&bindingExt,
			VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR,
			static_cast<uint32>(bindings[i].size()),
			bindings[i].data()
		};

		tmpLayouts[i] = (descriptorSetLayouts[i] = vk::DescriptorSetLayout(globalRenderSystem->device, &createInfo)).get();
	}

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		3,
		tmpLayouts.data(),
		0,
		nullptr
	};

	pipelineLayout = vk::PipelineLayout(globalRenderSystem->device, &pipelineLayoutCreateInfo);
}

Program::Program(const Shader& vertexShader, const Shader& fragmentShader, const Binder& binder) {

}

} // namespace vulkan

} // namespace lyra
