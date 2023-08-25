#include <Graphics/VulkanRenderSystem.h>

#include <Common/Utility.h>
#include <Common/Logger.h>
#include <Common/Config.h>

#include <Graphics/Mesh.h>
#include <Graphics/SDLWindow.h>

#include <SDL_vulkan.h>

#include <utility>
#include <limits>
#include <map>
#include <string>

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

	RenderSystem(const InitInfo& info) : window(info.window) {
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
			std::vector<const char*> instanceExtensions(info.window->getInstanceExtensions());
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

					VkPhysicalDeviceDescriptorIndexingProperties descriptorIndexingProperties {
						VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES
					};
					VkPhysicalDeviceProperties2KHR extendedProperties {
						VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR,
						&descriptorIndexingProperties
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

			physicalDevice = std::move(vk::PhysicalDevice(possibleDevices.rbegin()->second.device, instance));
			deviceProperties = possibleDevices.rbegin()->second.properties;
			descriptorIndexingProperties = possibleDevices.rbegin()->second.descriptorIndexingProperties;
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

#ifdef __APPLE__
			Dynarray<const char*, config::requestedDeviceExtensions.size() + 1> requestedExtensions(config::requestedDeviceExtensions.begin(), config::requestedDeviceExtensions.end());
			requestedExtensions.push_back("VK_KHR_portability_subset");
#else
			auto& requestedExtensions = config::requestedDeviceExtensions;
#endif

			VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptorIndexingFeatures { 
				VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT
			};
			descriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
			descriptorIndexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;
			descriptorIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
			descriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;

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
	VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vk::Device device;

	QueueFamilies queueFamilies;
	vk::Queue graphicsQueue;
	vk::Queue computeQueue;
	vk::Queue copyQueue;

	vma::Allocator allocator;

	const Window* window;
};

static RenderSystem* globalRenderSystem = nullptr;

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
	commandBuffer = vk::CommandBuffer(globalRenderSystem->device.get(), allocInfo);
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

CommandQueue::CommandQueue() : pipelineStageFlags({ VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }) { 
	commandPools.resize(config::maxFramesInFlight);
}

void CommandQueue::reset() {
	auto newCommandQueue = CommandQueue();
	std::swap(*this, newCommandQueue);
}

void CommandQueue::submit(VkFence fence, bool wait) {
	if (activeCommandBuffer != nullptr) {
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

		if (wait) {
			VULKAN_ASSERT(globalRenderSystem->waitForFence(vk::Fence(fence, VK_NULL_HANDLE), VK_TRUE, std::numeric_limits<uint32>::max()), "wait for fence to finish");
		}

		activeCommandBuffer = nullptr;

		currentFrame = (currentFrame + 1) % config::maxFramesInFlight;
	}

	waitSemaphores.clear();
	signalSemaphores.clear();
	// pipelineStageFlags.clear();
}

void CommandQueue::oneTimeBegin() {
	auto cmdBuff = CommandBuffer(commandPools.at(currentFrame));
	cmdBuff.begin(CommandQueue::CommandBuffer::Usage::oneTimeSubmit);
	activeCommandBuffer = &cmdBuff;
}

void CommandQueue::oneTimeSubmit() {
	activeCommandBuffer->end();
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
		getAllocCreateInfo(memUsage), 
		memory
	);
}

void GPUBuffer::copyData(const void* src, size_t copySize) {
	void* data;
	VULKAN_ASSERT(globalRenderSystem->mapMemory(memory, &data), "map buffer memory at {}", getAddress(memory));
	
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

void GPUBuffer::copyData(const void** src, uint32 arraySize, size_t elementSize) {
	char* data;
	VULKAN_ASSERT(globalRenderSystem->mapMemory(memory, (void**)&data), "map buffer memory at {}", getAddress(memory));

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
	commandQueue.activeCommandBuffer->copyBuffer(srcBuffer.buffer, buffer, copyRegion);

	commandQueue.oneTimeSubmit();
}

void Image::createView(VkFormat format, const VkImageSubresourceRange& subresourceRange, VkImageViewType viewType, const VkComponentMapping& colorComponents) {
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

void Image::transitionLayout(
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
	else ASSERT(false, "Invalid image layout transition was requested whilst transitioning an image layout at: {}!", getAddress(this));

	commandQueue.activeCommandBuffer->pipelineBarrier(
		sourceStage, 
		destinationStage, 
		0,
		VkMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM },
		VkBufferMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM }, 
		getImageMemoryBarrier(
			sourceAccess, 
			destinationAccess, 
			oldLayout, 
			newLayout, 
			subresourceRange
		)
	);

	commandQueue.oneTimeSubmit();
}

VkFormat Image::getBestFormat(const std::vector<VkFormat>& candidates, VkFormatFeatureFlags features, VkImageTiling tiling) {
	for (const auto& candidate : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(globalRenderSystem->physicalDevice, candidate, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) return candidate;
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) return candidate;
	}

	ASSERT(false, "Failed to find supported format out of user defined formats!");

	return VK_FORMAT_MAX_ENUM;
}

void Image::copyFromBuffer(const vulkan::GPUBuffer& stagingBuffer, const VkExtent3D& extent, uint32 layerCount) {
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
	commandQueue.activeCommandBuffer->copyBufferToImage(stagingBuffer.buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageCopy);

	commandQueue.oneTimeSubmit();
}

Swapchain::Swapchain(CommandQueue& commandQueue) : commandQueue(&commandQueue) {
	surface = vk::Surface(globalRenderSystem->instance, globalRenderSystem->window->get());

	{ // check present queue compatibility
		uint32 familyIndex = 0;
		for (const auto& queueFamilyProperty : globalRenderSystem->queueFamilies.queueFamilyProperties) {
			VkBool32 presentSupport;
			VULKAN_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(globalRenderSystem->physicalDevice, familyIndex, surface, &presentSupport),
				"check physical device queue family presentation support");
			
			if (presentFamilyIndex == std::numeric_limits<uint32>::max() && presentSupport == VK_TRUE) {
				presentFamilyIndex = familyIndex;
				presentQueue = vk::Queue(globalRenderSystem->device, presentFamilyIndex, 0);
				break;
			}

			familyIndex++;
		}

		if (presentFamilyIndex == std::numeric_limits<uint32>::max()) {
			ASSERT(false, "Failed to find queue family with presentation support with physical device: {} for surface: {}!", getAddress(globalRenderSystem->physicalDevice), getAddress(surface));
		}

		this->commandQueue->queue = presentQueue.get();
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
		auto drawableArea = globalRenderSystem->window->getDrawableSize();

		extent = VkExtent2D {
			static_cast<uint32>(std::clamp(drawableArea.x, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width)),
			static_cast<uint32>(std::clamp(drawableArea.t, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height))
		};
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
			(oldSwapchain) ? oldSwapchain : VK_NULL_HANDLE
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

			image.createView(format, { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
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
			colorImage.getImageCreateInfo(
				format,
				{ extent.width, extent.height, 1 },
				VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				1,
				VK_IMAGE_TYPE_2D,
				1,
				0,
				maxMultisamples
			),
			GPUMemory::getAllocCreateInfo(VMA_MEMORY_USAGE_GPU_ONLY, VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)),
			colorMem.memory
		);

		colorImage.createView(format, { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
	}
	
	{ // create depth images
		depthBufferFormat = Image::getBestFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_TILING_OPTIMAL);

		depthImage.image = vk::Image(
			globalRenderSystem->device,
			globalRenderSystem->allocator, 
			depthImage.getImageCreateInfo(
				depthBufferFormat,
				{ extent.width, extent.height, 1 },
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				1,
				VK_IMAGE_TYPE_2D,
				1,
				0,
				maxMultisamples
			),
			GPUMemory::getAllocCreateInfo(VMA_MEMORY_USAGE_GPU_ONLY, VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)),
			depthMem.memory
		);

		depthImage.createView(VK_FORMAT_D32_SFLOAT, { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 });
	}
}

bool Swapchain::update(bool windowChanged) {
	if (invalidAttachments || invalidSwapchain || lostSurface || windowChanged) {
		auto flags = globalRenderSystem->window->getWindowFlags();
		SDL_Event e;
		while ((flags & SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED && SDL_WaitEvent(&e)) {
			flags = globalRenderSystem->window->getWindowFlags();
		}

		vkDeviceWaitIdle(globalRenderSystem->device);

		for (auto& framebuffer : framebuffers) {
			framebuffer->destroyFramebuffers();
		}
		depthImage.destroy();
		depthMem.destroy();
		colorImage.destroy();
		colorMem.destroy();

		if (invalidSwapchain || lostSurface || globalRenderSystem->window->changed()) {
			if (oldSwapchain != nullptr) oldSwapchain.destroy();
			oldSwapchain = std::move(this->swapchain);

			for (auto& image : images) {
				image.view.destroy();
			}

			if (lostSurface) {
				surface = vk::Surface(globalRenderSystem->instance, globalRenderSystem->window->get());
			}

			createSwapchain();
		}

		createAttachments();
		for (auto& framebuffer : framebuffers) {
			framebuffer->createFramebuffers();
		}

		lostSurface = false;
		invalidSwapchain = false;
		invalidAttachments = false;

		return true;
	}

	return false;
}

bool Swapchain::aquire() {
	currentFrame = commandQueue->currentFrame;

	globalRenderSystem->waitForFence(renderFinishedFences[currentFrame], VK_TRUE, std::numeric_limits<uint64>::max());

	VkResult result = vkAcquireNextImageKHR(
		globalRenderSystem->device, 
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

	return !update();
}

void Swapchain::begin() {
	globalRenderSystem->resetFence(renderFinishedFences[currentFrame]);

	commandQueue->activeCommandBuffer = &(commandBuffer = std::move(lyra::vulkan::CommandQueue::CommandBuffer(commandQueue->commandPools[commandQueue->currentFrame])));
	commandQueue->waitSemaphores.push_back(imageAquiredSemaphores[currentFrame]);
	commandQueue->signalSemaphores.push_back(submitFinishedSemaphores[currentFrame]);
}

bool Swapchain::present() {
	VkPresentInfoKHR presentInfo {
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1,
		&submitFinishedSemaphores[currentFrame].get(),
		1,
		&swapchain.get(),
		&imageIndex
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

	return !update(globalRenderSystem->window->changed());
}

Framebuffers::Framebuffers(Swapchain& swapchain) : swapchain(&swapchain) {
	swapchain.framebuffers.insert(this);

	{ // create the render pass
		VkAttachmentDescription colorAttachmentDescriptions {
			0,
			swapchain.format,
			swapchain.maxMultisamples,
			VK_ATTACHMENT_LOAD_OP_CLEAR,
			VK_ATTACHMENT_STORE_OP_STORE,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		VkAttachmentDescription	depthBufferAttachmentDescriptions {
			0,
			swapchain.depthBufferFormat,
			swapchain.maxMultisamples,
			VK_ATTACHMENT_LOAD_OP_CLEAR,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};

		VkAttachmentDescription colorAttachmentFinalDescriptions { 
			0,
			swapchain.format,
			VK_SAMPLE_COUNT_1_BIT,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_STORE,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};

		VkAttachmentReference colorAttachmentReferences {
			0,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		VkAttachmentReference depthBufferAttachmentReferences {
			1,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};

		VkAttachmentReference colorAttachmentFinalReferences {
			2,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		Array<VkAttachmentDescription, 3> attachments = {{ colorAttachmentDescriptions, depthBufferAttachmentDescriptions, colorAttachmentFinalDescriptions }};

		VkSubpassDescription subpassDescriptions {
			0,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			0,
			nullptr,
			1,
			&colorAttachmentReferences,
			&colorAttachmentFinalReferences,
			&depthBufferAttachmentReferences,
			0,
			nullptr
		};

		VkSubpassDependency dependencies {
			VK_SUBPASS_EXTERNAL,
			0,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			0,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			0
		};

		// create the render pass
		VkRenderPassCreateInfo createInfo {
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			nullptr,
			0,
			static_cast<uint32>(attachments.size()),
			attachments.data(),
			1,
			&subpassDescriptions,
			1,
			&dependencies
		};

		renderPass = vk::RenderPass(globalRenderSystem->device, createInfo);
	}

	framebuffers.resize(swapchain.images.size());
	createFramebuffers();
}

Framebuffers::~Framebuffers() {
	swapchain->framebuffers.erase(this);
}

void Framebuffers::createFramebuffers() { // create the framebuffers
	for (uint32 i = 0; i < framebuffers.size(); i++) {
		Array<VkImageView, 3> attachments = { {
			swapchain->colorImage.view,
			swapchain->depthImage.view,
			swapchain->images[i].view
		} };

		// create the frame buffers
		VkFramebufferCreateInfo createInfo{
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			nullptr,
			0,
			renderPass,
			static_cast<uint32>(attachments.size()),
			attachments.data(),
			swapchain->extent.width,
			swapchain->extent.height,
			1
		};

		framebuffers[i] = vk::Framebuffer(globalRenderSystem->device, createInfo);
	}
}

void Framebuffers::begin() const {
	static constexpr Array<VkClearValue, 2> clear {{
		{
			{ 0.0f, 0.0f, 0.0f, 0.0f },
		},
		{
			{ 1.0f, 0 }
		}
	}};

	VkRenderPassBeginInfo beginInfo{
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		nullptr,
		renderPass,
		framebuffers[swapchain->imageIndex],
		{	// rendering area
			{ 0, 0 },
			swapchain->extent
		},
		clear.size(),
		clear.data()
	};

	swapchain->commandQueue->activeCommandBuffer->beginRenderPass(beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Framebuffers::end() const {
	swapchain->commandQueue->activeCommandBuffer->endRenderPass();
}

Shader::Shader(Type type, std::vector<char>&& source) : shaderSrc(source), type(type) {
	VkShaderModuleCreateInfo createInfo{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		shaderSrc.size(),
		reinterpret_cast<const uint32*>(shaderSrc.data())
	};

	module = vk::ShaderModule(globalRenderSystem->device, createInfo);
}

Shader::Shader(Type type, const std::vector<char>& source) : shaderSrc(source), type(type) {
	VkShaderModuleCreateInfo createInfo{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		shaderSrc.size(),
		reinterpret_cast<const uint32*>(shaderSrc.data())
	};

	module = vk::ShaderModule(globalRenderSystem->device, createInfo);
}

GraphicsProgram::GraphicsProgram(const Shader& vertexShader, const Shader& fragmentShader) : vertexShader(&vertexShader), fragmentShader(&fragmentShader) {
	static constexpr VkDescriptorBindingFlags bindingFlag = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
	static constexpr Array<VkDescriptorSetLayoutBindingFlagsCreateInfo, 4> bindingExt {{
		{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO },
		{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO },
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
			nullptr,
			1,
			&bindingFlag
		},
		{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO }
	}};
	static constexpr Array<Dynarray<VkDescriptorSetLayoutBinding, 6>, 4> bindings {{
		{{
			{
				0,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				1,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			}
		}},
		{{
			{
				0,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				1,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			},
			{ 
				1,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				1,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			},
			{ 
				2,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				1,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			},
			{ 
				3,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				1,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			},
			{ 
				4,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				1,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			},
			{ 
				5,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				1,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			},
		}},
		{{
			{ 
				0,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				config::maxTexturesPerBinding,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			}
		}},
		{{
			{
				0,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				1,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			}
		}}
	}};

	Array<VkDescriptorSetLayout, 4> tmpLayouts;

	descriptorSetLayouts.resize(4);

	for (uint32 i = 0; i < 4; i++) {
		VkDescriptorSetLayoutCreateInfo createInfo{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			&bindingExt[i],
			VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
			static_cast<uint32>(bindings[i].size()),
			bindings[i].data()
		};

		if (i == 2 && (config::maxTexturesPerBinding + 5 >= globalRenderSystem->descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSamplers)) { // small hack, won't work on MacOS otherwise {
			auto data = bindings[i];
			data[0].descriptorCount = (globalRenderSystem->descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSamplers - 5);

			createInfo.pBindings = data.data();
		}

		tmpLayouts[i] = (descriptorSetLayouts[i] = vk::DescriptorSetLayout(globalRenderSystem->device, createInfo)).get();
	}

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		tmpLayouts.size(),
		tmpLayouts.data(),
		0,
		nullptr
	};

	pipelineLayout = vk::PipelineLayout(globalRenderSystem->device, pipelineLayoutCreateInfo);

	/*
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo {
		VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		nullptr,
		0
	};

	pipelineCache = vk::PipelineCache(globalRenderSystem->device, pipelineCacheCreateInfo);
	*/
}

GraphicsProgram::GraphicsProgram(const Shader& vertexShader, const Shader& fragmentShader, const Binder& binder) {
	uint32 setCount = binder.m_bindings.size();

	std::vector<VkDescriptorSetLayout> tmpLayouts(setCount);

	descriptorSetLayouts.resize(setCount);

	for (uint32 i = 0; i < setCount; i++) {
		VkDescriptorSetLayoutCreateInfo createInfo{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			&binder.m_bindingFlagsCreateInfo[i],
			VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR,
			static_cast<uint32>(binder.m_bindings[i].size()),
			binder.m_bindings[i].data()
		};

		tmpLayouts[i] = (descriptorSetLayouts[i] = vk::DescriptorSetLayout(globalRenderSystem->device, createInfo)).get();
	}

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		setCount,
		tmpLayouts.data(),
		static_cast<uint32>(binder.m_pushConstants.size()),
		binder.m_pushConstants.data()
	};

	pipelineLayout = vk::PipelineLayout(globalRenderSystem->device, pipelineLayoutCreateInfo);
}

DescriptorPools::DescriptorPools(const std::vector<Size>& sizes, Flags flags) {
	constexpr static uint32 descriptorCount = config::descriptorPoolAllocCount * config::maxFramesInFlight;

	for (const auto& size : sizes) {
		this->sizes.push_back({
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

	descriptorPools.emplace_back(globalRenderSystem->device, createInfo);
}

void DescriptorPools::reset() {
	for (auto& pool : descriptorPools)
		globalRenderSystem->resetDescriptorPool(pool, 0);
}

void DescriptorPools::allocAndBind(
	const CommandQueue::CommandBuffer& cmdBuff, 
	const DescriptorWriter& writer, 
	const GraphicsProgram& program,
	uint32 layoutIndex
) {
	VkDescriptorSetAllocateInfo allocInfo {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		descriptorPools.back(),
		1,
		&program.descriptorSetLayouts[layoutIndex].get()
	};

	vk::DescriptorSet descriptorSet;
	VkResult result = vkAllocateDescriptorSets(globalRenderSystem->device, &allocInfo, &descriptorSet.get());

	if (result == VK_ERROR_OUT_OF_POOL_MEMORY) {
		allocInfo.descriptorPool = descriptorPools.emplace_back(globalRenderSystem->device, createInfo).get();
		result = vkAllocateDescriptorSets(globalRenderSystem->device, &allocInfo, &descriptorSet.get());
	}

	VULKAN_ASSERT(result, "allocate Vulkan descriptor set");

	cmdBuff.bindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, program.pipelineLayout, 0, descriptorSet, 0);
}

GraphicsPipeline::Builder::Builder(const Swapchain& swapchain, const Framebuffers& renderer) :
	m_renderer(&renderer) { 
	m_createInfo = GraphicsPipelineCreateInfo {
		// shader information
		Mesh::Vertex::getBindingDescription(),
		Mesh::Vertex::getAttribute_descriptions(),
		{	// describe how vertices are inputed into shaders
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			0,
			1,
			&m_createInfo.meshBindingDescription,
			static_cast<uint32>(m_createInfo.meshAttributeDescriptions.size()),
			m_createInfo.meshAttributeDescriptions.data()
		},
		{	// describe how shaders are executed
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_FALSE
		},
		{	// tesselation

		},
		false,
		false,
		{	// create the rasteriser to create the fragments
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
		},
		{	// configure anti alising
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			nullptr,
			0,
			swapchain.maxMultisamples,
			VK_FALSE,
			0.0f
		},
		{	// depth buffering
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_TRUE,
			VK_TRUE,
			VK_COMPARE_OP_LESS,
			VK_FALSE,
			VK_FALSE
		},
		{	// configure color blending
			VK_FALSE,
			VK_BLEND_FACTOR_SRC_ALPHA,
			VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			VK_BLEND_OP_ADD,
			VK_BLEND_FACTOR_ONE,
			VK_BLEND_FACTOR_ZERO,
			VK_BLEND_OP_ADD,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		},
		{
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_FALSE,
			VK_LOGIC_OP_COPY,
			1,
			&m_createInfo.colorBlendAttachment,
			{
				0.0f, 0.0f, 0.0f, 0.0f
			}
		}
	};
}

GraphicsPipeline::GraphicsPipeline(const GraphicsProgram& program, const Builder& builder) {
	VkPipelineViewportStateCreateInfo viewportState {
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		nullptr,
		0,
		0,
		nullptr,
		0,
		nullptr
	};

	// configure dynamic state
	Dynarray<VkDynamicState, 2> dynamicState;
	if (std::holds_alternative<bool>(builder.m_createInfo.viewport)) {
		dynamicState.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	} else {
		viewportState.viewportCount = 1;
		viewportState.pViewports = &std::get<VkViewport>(builder.m_createInfo.viewport);
	} if (std::holds_alternative<bool>(builder.m_createInfo.scissor)) {
		dynamicState.push_back(VK_DYNAMIC_STATE_SCISSOR);
	} else {
		viewportState.scissorCount = 1;
		viewportState.pScissors = &std::get<VkRect2D>(builder.m_createInfo.scissor);
	} 

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo {
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32>(dynamicState.size()),
		dynamicState.data()
	};

	Array<VkPipelineShaderStageCreateInfo, 2> tmpShaders = { program.vertexShader->getStageCreateInfo(), program.fragmentShader->getStageCreateInfo() };

	VkGraphicsPipelineCreateInfo createInfo {
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,	
		nullptr,
		0,
		2,
		tmpShaders.data(),
		&builder.m_createInfo.vertexInputInfo,
		&builder.m_createInfo.inputAssembly,
		&builder.m_createInfo.tesselation,
		&viewportState,
		&builder.m_createInfo.rasterizer,
		&builder.m_createInfo.multisampling,
		&builder.m_createInfo.depthStencilState,
		&builder.m_createInfo.colorBlending,
		&dynamicStateCreateInfo,
		program.pipelineLayout,
		builder.m_renderer->renderPass,
		0,
		VK_NULL_HANDLE,
		0
	};

	pipeline = vk::GraphicsPipeline(globalRenderSystem->device, program.pipelineCache, createInfo);
}

} // namespace vulkan

void initRenderSystem(const vulkan::InitInfo& info) {
	vulkan::globalRenderSystem = new vulkan::RenderSystem(info);
}

void quitRenderSystem() {
	if (vulkan::globalRenderSystem) vkDeviceWaitIdle(vulkan::globalRenderSystem->device);
}

} // namespace lyra
