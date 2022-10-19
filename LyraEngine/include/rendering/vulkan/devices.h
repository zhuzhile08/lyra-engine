/*************************
 * @file devices.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a wrapper around the Vulkan devices
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <vector>
#include <map>

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

namespace lyra {

namespace vulkan {

/**
 * @brief Vulkan physical and logical devices
 */
class Device {
public:
	/**
	 * @brief queue families
	 */
	struct QueueFamily {
		VkQueue queue = VK_NULL_HANDLE;
		uint32  familyIndex = 0;
	};

	/**
	 * @brief create the devices
	 */
	Device();
	/**
	* @brief destructor of the device
	**/
	virtual ~Device();
	/**
	 * @brief No copy constructors?
	 * @param ⠀⣞⢽⢪⢣⢣⢣⢫⡺⡵⣝⡮⣗⢷⢽⢽⢽⣮⡷⡽⣜⣜⢮⢺⣜⢷⢽⢝⡽⣝
	 * @param ⠸⡸⠜⠕⠕⠁⢁⢇⢏⢽⢺⣪⡳⡝⣎⣏⢯⢞⡿⣟⣷⣳⢯⡷⣽⢽⢯⣳⣫⠇
	 * @param ⠀⠀⢀⢀⢄⢬⢪⡪⡎⣆⡈⠚⠜⠕⠇⠗⠝⢕⢯⢫⣞⣯⣿⣻⡽⣏⢗⣗⠏⠀
	 * @param ⠀⠪⡪⡪⣪⢪⢺⢸⢢⢓⢆⢤⢀⠀⠀⠀⠀⠈⢊⢞⡾⣿⡯⣏⢮⠷⠁⠀⠀
	 * @param ⠀⠀⠀⠈⠊⠆⡃⠕⢕⢇⢇⢇⢇⢇⢏⢎⢎⢆⢄⠀⢑⣽⣿⢝⠲⠉⠀⠀⠀⠀
	 * @param ⠀⠀⠀⠀⠀⡿⠂⠠⠀⡇⢇⠕⢈⣀⠀⠁⠡⠣⡣⡫⣂⣿⠯⢪⠰⠂⠀⠀⠀⠀
	 * @param ⠀⠀⠀⠀⡦⡙⡂⢀⢤⢣⠣⡈⣾⡃⠠⠄⠀⡄⢱⣌⣶⢏⢊⠂⠀⠀⠀⠀⠀⠀
	 * @param ⠀⠀⠀⠀⢝⡲⣜⡮⡏⢎⢌⢂⠙⠢⠐⢀⢘⢵⣽⣿⡿⠁⠁⠀⠀⠀⠀⠀⠀⠀
	 * @param ⠀⠀⠀⠀⠨⣺⡺⡕⡕⡱⡑⡆⡕⡅⡕⡜⡼⢽⡻⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
	 * @param ⠀⠀⠀⠀⣼⣳⣫⣾⣵⣗⡵⡱⡡⢣⢑⢕⢜⢕⡝⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
	 * @param ⠀⠀⠀⣴⣿⣾⣿⣿⣿⡿⡽⡑⢌⠪⡢⡣⣣⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
	 * @param ⠀⠀⠀⡟⡾⣿⢿⢿⢵⣽⣾⣼⣘⢸⢸⣞⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
	 * @param⠀⠀⠀⠀⠁⠇⠡⠩⡫⢿⣝⡻⡮⣒⢽⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
	 */
	Device operator=(const Device&) const noexcept = delete;

	/**
	 * @brief wait for the logical device to finish with whatever operations are still going on
	 */
	void wait() const { vassert(vkDeviceWaitIdle(m_device), "wait for device to finish its operations"); }

	/**
	 * @brief wrappers around the core Vulkan API and VMA functions
	 * @brief these are basically copied directly from the Vulkan API with minor modifications to reduce bloat and suit a more modern C++ style
	 * @brief please refer to the official Vulkan documentation (preferably at https://devdocs.io/vulkan/ and https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/) for the documentation of these functions
	 */

	
	VkResult createPool(const VmaPoolCreateInfo& pCreateInfo, VmaPool& pPool) {
		return vmaCreatePool(m_allocator, &pCreateInfo, &pPool);
	}
	VkResult createBuffer(const VkBufferCreateInfo& pBufferCreateInfo, const VmaAllocationCreateInfo& pAllocationCreateInfo, VkBuffer& pBuffer, VmaAllocation& pAllocation, VmaAllocationInfo pAllocationInfo = VmaAllocationInfo{}) {
		return vmaCreateBuffer(m_allocator, &pBufferCreateInfo, &pAllocationCreateInfo, &pBuffer, &pAllocation, &pAllocationInfo);
	}
	VkResult createBufferWithAlignment(const VkBufferCreateInfo& pBufferCreateInfo, const VmaAllocationCreateInfo& pAllocationCreateInfo, VkDeviceSize minAlignment, VkBuffer& pBuffer, VmaAllocation& pAllocation, VmaAllocationInfo& pAllocationInfo) {
		return vmaCreateBufferWithAlignment(m_allocator, &pBufferCreateInfo, &pAllocationCreateInfo, minAlignment, &pBuffer, &pAllocation, &pAllocationInfo);
	}
	VkResult createAliasingBuffer(VmaAllocation allocation, const VkBufferCreateInfo& pBufferCreateInfo, VkBuffer& pBuffer) {
		return vmaCreateAliasingBuffer(m_allocator, allocation, &pBufferCreateInfo, &pBuffer);
	}
	// VkResult createAliasingBuffer2(VmaAllocation allocation, VkDeviceSize allocationLocalOffset, const VkBufferCreateInfo& pBufferCreateInfo, VkBuffer& pBuffer) {
	// 	vmaCreateAliasingBuffer2(m_allocator, allocation, allocationLocalOffset, &pBufferCreateInfo, &pBuffer);
	// }
	VkResult createImage(const VkImageCreateInfo& pImageCreateInfo, const VmaAllocationCreateInfo& pAllocationCreateInfo, VkImage& pImage, VmaAllocation& pAllocation, VmaAllocationInfo pAllocationInfo = VmaAllocationInfo{}) {
		return vmaCreateImage(m_allocator, &pImageCreateInfo, &pAllocationCreateInfo, &pImage, &pAllocation, &pAllocationInfo);
	}
	VkResult createAliasingImage(VmaAllocation allocation, const VkImageCreateInfo& pImageCreateInfo, VkImage& pImage) {
		return vmaCreateAliasingImage(m_allocator, allocation, &pImageCreateInfo, &pImage);
	}
	// VkResult createAliasingImage2(VmaAllocation allocation, VkDeviceSize allocationLocalOffset, const VkImageCreateInfo& pImageCreateInfo, VkImage& pImage) {
	// 	vmaCreateAliasingImage2(m_allocator, allocationLocalOffset, &pImageCreateInfo, &pImage);
	// }
	void destroyPool(VmaPool pool) {
		vmaDestroyPool(m_allocator, pool);
	}
	void destroyBuffer(VkBuffer buffer, VmaAllocation allocation) {
		vmaDestroyBuffer(m_allocator, buffer, allocation);
	}
	void destroyImage(VkImage image, VmaAllocation allocation) {
		vmaDestroyImage(m_allocator, image, allocation);
	}
	void freeCommandBuffers(VkCommandPool commandPool, uint32 commandBufferCount, const VkCommandBuffer& pCommandBuffers) {
		vkFreeCommandBuffers(m_device, commandPool, commandBufferCount, &pCommandBuffers);
	}
	VkResult freeDescriptorSets(VkDescriptorPool descriptorPool, uint32 descriptorSetCount, const VkDescriptorSet& pDescriptorSets) {
		return vkFreeDescriptorSets(m_device, descriptorPool, descriptorSetCount, &pDescriptorSets);
	}
	VkResult findMemoryTypeIndexForBufferInfo(const VkBufferCreateInfo& pBufferCreateInfo, const VmaAllocationCreateInfo& pAllocationCreateInfo, uint32& pMemoryTypeIndex) {
		return vmaFindMemoryTypeIndexForBufferInfo(m_allocator, &pBufferCreateInfo, &pAllocationCreateInfo, &pMemoryTypeIndex);
	}
	VkResult findMemoryTypeIndexForImageInfo(const VkImageCreateInfo& pImageCreateInfo, const VmaAllocationCreateInfo& pAllocationCreateInfo, uint32& pMemoryTypeIndex) {
		return vmaFindMemoryTypeIndexForImageInfo(m_allocator, &pImageCreateInfo, &pAllocationCreateInfo, &pMemoryTypeIndex);
	}
	VkResult findMemoryTypeIndex(uint32 memoryTypeBits, const VmaAllocationCreateInfo& pAllocationCreateInfo, uint32& pMemoryTypeIndex) {
		return vmaFindMemoryTypeIndex(m_allocator, memoryTypeBits, &pAllocationCreateInfo, &pMemoryTypeIndex);
	}
	VkResult checkPoolCorruption(VmaPool pool) {
		return vmaCheckPoolCorruption(m_allocator, pool);
	}
	void getPoolName(VmaPool pool, const char** ppName) {
		vmaGetPoolName(m_allocator, pool, ppName);
	}
	void setPoolName(VmaPool pool, const char* pName) {
		vmaSetPoolName(m_allocator, pool, pName);
	}
	VkResult allocateMemoryPages(const VkMemoryRequirements& pMemoryRequirements, const VmaAllocationCreateInfo& pCreateInfo, VmaAllocation& pAllocations, VmaAllocationInfo& pAllocationInfo) {
		return vmaAllocateMemoryPages(m_allocator, &pMemoryRequirements, &pCreateInfo, 1, &pAllocations, &pAllocationInfo);
	}
	VkResult allocateMemoryForBuffer(VkBuffer buffer, const VmaAllocationCreateInfo& pCreateInfo, VmaAllocation& pAllocation, VmaAllocationInfo& pAllocationInfo) {
		return vmaAllocateMemoryForBuffer(m_allocator, buffer, &pCreateInfo, &pAllocation, &pAllocationInfo);
	}
	VkResult allocateMemoryForImage(VkImage image, const VmaAllocationCreateInfo& pCreateInfo, VmaAllocation& pAllocation, VmaAllocationInfo& pAllocationInfo) {
		return vmaAllocateMemoryForImage(m_allocator, image, &pCreateInfo, &pAllocation, &pAllocationInfo);
	}
	void freeMemoryPages(size_t allocationCount, const VmaAllocation& pAllocations) {
		vmaFreeMemoryPages(m_allocator, allocationCount, &pAllocations);
	}
	void getAllocationInfo(VmaAllocation allocation, VmaAllocationInfo& pAllocationInfo) {
		vmaGetAllocationInfo(m_allocator, allocation, &pAllocationInfo);
	}
	void setAllocationUserData(VmaAllocation allocation, void* pUserData) {
		vmaSetAllocationUserData(m_allocator, allocation, pUserData);
	}
	void setAllocationName(VmaAllocation allocation, const char* pName) {
		vmaSetAllocationName(m_allocator, allocation, pName);
	}
	void getAllocationMemoryProperties(VmaAllocation allocation, VkMemoryPropertyFlags& pFlags) {
		vmaGetAllocationMemoryProperties(m_allocator, allocation, &pFlags);
	}
	VkResult flushAllocation(VmaAllocation allocation, VkDeviceSize offset, VkDeviceSize size) {
		return vmaFlushAllocation(m_allocator, allocation, offset, size);
	}
	VkResult invalidateAllocation(VmaAllocation allocation, VkDeviceSize offset, VkDeviceSize size) {
		return vmaInvalidateAllocation(m_allocator, allocation, offset, size);
	}
	VkResult flushAllocations(uint32 allocationCount, const VmaAllocation& allocations, const VkDeviceSize& offsets, const VkDeviceSize& sizes) {
		return vmaFlushAllocations(m_allocator, allocationCount, &allocations, &offsets, &sizes);
	}
	VkResult invalidateAllocations(uint32 allocationCount, const VmaAllocation& allocations, const VkDeviceSize& offsets, const VkDeviceSize& sizes) {
		return vmaInvalidateAllocations(m_allocator, allocationCount, &allocations, &offsets, &sizes);
	}
	VkResult checkCorruption(uint32 memoryTypeBits) {
		return vmaCheckCorruption(m_allocator, memoryTypeBits);
	}
	VkResult beginDefragmentation(const VmaDefragmentationInfo& pInfo, VmaDefragmentationContext& pContext) {
		return vmaBeginDefragmentation(m_allocator, &pInfo, &pContext);
	}
	void endDefragmentation(VmaDefragmentationContext context, VmaDefragmentationStats& pStats) {
		vmaEndDefragmentation(m_allocator, context, &pStats);
	}
	VkResult beginDefragmentationPass(VmaDefragmentationContext context, VmaDefragmentationPassMoveInfo& pPassInfo) {
		return vmaBeginDefragmentationPass(m_allocator, context, &pPassInfo);
	}
	VkResult endDefragmentationPass(VmaDefragmentationContext context, VmaDefragmentationPassMoveInfo& pPassInfo) {
		return vmaEndDefragmentationPass(m_allocator, context, &pPassInfo);
	}
	VkResult bindBufferMemory(VmaAllocation allocation, VkBuffer buffer) {
		return vmaBindBufferMemory(m_allocator, allocation, buffer);
	}
	VkResult bindBufferMemory2(VmaAllocation allocation, VkDeviceSize allocationLocalOffset, VkBuffer buffer, const void* pNext) {
		return vmaBindBufferMemory2(m_allocator, allocation, allocationLocalOffset, buffer, pNext);
	}
	VkResult bindImageMemory(VmaAllocation allocation, VkImage image) {
		return vmaBindImageMemory(m_allocator, allocation, image);
	}
	VkResult bindImageMemory2(VmaAllocation allocation, VkDeviceSize allocationLocalOffset, VkImage image, const void* pNext) {
		return vmaBindImageMemory2(m_allocator, allocation, allocationLocalOffset, image, pNext);
	}
	void freeMemory(const VmaAllocation allocation) {
		vmaFreeMemory(m_allocator, allocation);
	}
	VkResult flushMappedMemoryRanges(uint32 memoryRangeCount, const VkMappedMemoryRange& pMemoryRanges) {
		return vkFlushMappedMemoryRanges(m_device, memoryRangeCount, &pMemoryRanges);
	}
	void getDeviceQueue(uint32 queueFamilyIndex, uint32 queueIndex, VkQueue& pQueue) {
		vkGetDeviceQueue(m_device, queueFamilyIndex, queueIndex, &pQueue);
	}
	VkResult getEventStatus(VkEvent event) {
		return vkGetEventStatus(m_device, event);
	}
	VkResult getFenceStatus(VkFence fence) {
		return vkGetFenceStatus(m_device, fence);
	}
	void getImageMemoryRequirements(VkImage image, VkMemoryRequirements& pMemoryRequirements) {
		vkGetImageMemoryRequirements(m_device, image, &pMemoryRequirements);
	}
	void getImageSparseMemoryRequirements(VkImage image, uint32& pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements& pSparseMemoryRequirements) {
		vkGetImageSparseMemoryRequirements(m_device, image, &pSparseMemoryRequirementCount, &pSparseMemoryRequirements);
	}
	void getImageSubresourceLayout(VkImage image, const VkImageSubresource& pSubresource, VkSubresourceLayout& pLayout) {
		vkGetImageSubresourceLayout(m_device, image, &pSubresource, &pLayout);
	}
	// physical device functions
	VkResult getPipelineCacheData(VkPipelineCache pipelineCache, size_t& pDataSize, void* pData) {
		return vkGetPipelineCacheData(m_device, pipelineCache, &pDataSize, pData);
	}
	VkResult getQueryPoolResults(VkQueryPool queryPool, uint32 firstQuery, uint32 queryCount, size_t dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags) {
		return vkGetQueryPoolResults(m_device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
	}
	void getRenderAreaGranularity(VkRenderPass renderPass, VkExtent2D& pGranularity) {
		vkGetRenderAreaGranularity(m_device, renderPass, &pGranularity);
	}
	VkResult invalidateMappedMemoryRanges(uint32 memoryRangeCount, const VkMappedMemoryRange& pMemoryRanges) {
		return vkInvalidateMappedMemoryRanges(m_device, memoryRangeCount, &pMemoryRanges);
	}
	VkResult mapMemory(VmaAllocation allocation, void** ppData) {
		return vmaMapMemory(m_allocator, allocation, ppData);
	}
	VkResult mergePipelineCaches(VkPipelineCache dstCache, uint32 srcCacheCount, const VkPipelineCache& pSrcCaches) {
		return vkMergePipelineCaches(m_device, dstCache, srcCacheCount, &pSrcCaches);
	}
	VkResult resetCommandPool(VkCommandPool commandPool, VkCommandPoolResetFlags flags) {
		return vkResetCommandPool(m_device, commandPool, flags);
	}
	VkResult resetDescriptorPool(VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags) {
		return vkResetDescriptorPool(m_device, descriptorPool, flags);
	}
	VkResult resetEvent(VkEvent event) {
		return vkResetEvent(m_device, event);
	}
	VkResult resetFences(uint32 fenceCount, const VkFence& pFences) {
		return vkResetFences(m_device, fenceCount, &pFences);
	}
	VkResult setEvent(VkEvent event) {
		return vkSetEvent(m_device, event);
	}
	void unmapMemory(VmaAllocation allocation) {
		vmaUnmapMemory(m_allocator, allocation);
	}
	void updateDescriptorSets(uint32 descriptorWriteCount, const VkWriteDescriptorSet& pDescriptorWrites, uint32 descriptorCopyCount, const VkCopyDescriptorSet& pDescriptorCopies) {
		vkUpdateDescriptorSets(m_device, descriptorWriteCount, &pDescriptorWrites, descriptorCopyCount, &pDescriptorCopies);
	}
	VkResult waitForFences(uint32 fenceCount, const VkFence& pFences, VkBool32 waitAll, uint64 timeout) {
		return vkWaitForFences(m_device, fenceCount, &pFences, waitAll, timeout);
	}

	// don't ask me how long this took

	/**
	 * @brief get the Vulkan instance
	 *
	 * @return const VkInstance&
	 */
	NODISCARD const VkInstance& instance() const noexcept { return m_instance; }
	/**
	 * @brief get the GPU
	 *
	 * @return VkPhysicalDevice
	 */
	NODISCARD const VkPhysicalDevice& physicalDevice() const noexcept { return m_physicalDevice;  }
	/**
	 * @brief get the logical device
	 * 
	 * @return const VkLogicalDevice&
	 */
	NODISCARD const VkDevice& device() const noexcept { return m_device; }
	/**
	 * @brief get the graphics queue
	 * 
	 * @return const lyra::QueueFamily&
	 */
	NODISCARD const QueueFamily& graphicsQueue() const noexcept { return m_graphicsQueue; }
	/**
	 * @brief get the presentation queue
	 *
	 * @return const lyra::QueueFamily&
	 */
	NODISCARD const QueueFamily& presentQueue() const noexcept { return m_presentQueue; }
	/**
	 * @brief get the VMA memory allocator
	 *
	 * @return const VmaAllocator&
	 */
	NODISCARD const VmaAllocator& allocator() const noexcept { return m_allocator; }

private:
	VkInstance m_instance = VK_NULL_HANDLE;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_device = VK_NULL_HANDLE;

	QueueFamily m_graphicsQueue;
	QueueFamily m_presentQueue;

	VmaAllocator m_allocator = VK_NULL_HANDLE;

	/**
	 * @brief check if a vector of user requested Vulkan validation layers is actually available
	 *
	 * @param layers the available validation layers
	 * @param requestedLayers the requested validation layers
	 */
	void check_requested_validation_layers(const std::vector <VkLayerProperties>& layers, const std::vector <const char*>& requestedLayers) const;
	/**
	 * @brief check requested Vulkan device extensions
	 *
	 * @param extensions the available extensions
	 * @param requestedExtensions the requested extensions
	 */
	void check_requested_extensions(const std::vector <VkExtensionProperties> extensions, const std::vector <const char*> requestedExtensions) const;

	/**
	 * @brief find the family index of a queues
	 *
	 * @param queue the queue to find the family index of
	 * @param device device to find the family index of the queue of
	 * @return QueueFamily
	 */
	void find_family_index(QueueFamily* const queue, const VkPhysicalDevice device) noexcept;

	/**
	 * @brief rate a physical device by its features
	 *
	 * @param device the device to rate
	 * @param map a map containing all the physical devices and their scores
	 */
	void rate_physical_device(const VkPhysicalDevice& device, std::multimap <int, VkPhysicalDevice>& map);

	/**
	 * @brief create a Vulkan queue
	 *
	 * @return QueueFamily
	 */
	void create_queue(QueueFamily* const queue) noexcept;
	
	/**
	 * @brief create a Vulkan instance
	 *
	 * @param window window
	 */
	void create_instance();
	/**
	 * @brief select a physical device from the available ones
	 */
	void pick_physical_device();
	/**
	 * @brief create a logical device
	 */
	void create_logical_device();
	/**
	 * @brief create the VMA memoryW allocator
	 */
	void create_allocator();
	
	friend class RenderSystem;
};

} // namespace vulkan

} // namespace lyra
