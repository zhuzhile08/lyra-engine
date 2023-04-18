/*************************
 * @file Device.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a wrapper around the Vulkan devices
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Lyra/Lyra.h>

#include <limits>
#include <vector>
#include <map>

#include <vulkan/vulkan.h>
#include <Common/RAIIContainers.h>

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
private:
	// structure containing the queue indices of the device
	struct QueueFamilies {
		uint32 graphicsComputeQueueIndex = std::numeric_limits<uint32>::max();
		uint32 presentQueueIndex = std::numeric_limits<uint32>::max();
		// uint32 computeQueueIndex;

		constexpr bool found() const noexcept {
			return ((graphicsComputeQueueIndex != std::numeric_limits<uint32>::max()) && (presentQueueIndex != std::numeric_limits<uint32>::max()));
		}
	};

public:
	/**
	 * @brief create the devices
	 */
	Device();
	/**
	* @brief destructor of the device
	**/
	virtual ~Device() { wait(); }

	/**
	 * @brief wait for the logical device to finish with whatever operations are still going on
	 */
	void wait() const { vassert(vkDeviceWaitIdle(m_device), "wait for device to finish its operations"); }

	/**
	 * @brief wrappers around the core Vulkan API and VMA functions
	 * @brief these are basically copied directly from the Vulkan API with minor modifications to reduce bloat and suit a more modern C++ style
	 * @brief please refer to the official Vulkan documentation (preferably at https:/devdocs.io// and https:/gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/) for the documentation of these functions
	 */

		
	DEPRECATED VkResult createPool(const VmaPoolCreateInfo& pCreateInfo, VmaPool& pPool) {
		return vmaCreatePool(m_allocator, &pCreateInfo, &pPool); /// @todo
	}
	DEPRECATED VkResult createBuffer(const VkBufferCreateInfo& pBufferCreateInfo, const VmaAllocationCreateInfo& pAllocationCreateInfo, VkBuffer& pBuffer, VmaAllocation& pAllocation, VmaAllocationInfo pAllocationInfo = VmaAllocationInfo{}) {
		return vmaCreateBuffer(m_allocator, &pBufferCreateInfo, &pAllocationCreateInfo, &pBuffer, &pAllocation, &pAllocationInfo); /// @todo
	}
	DEPRECATED VkResult createBufferWithAlignment(const VkBufferCreateInfo& pBufferCreateInfo, const VmaAllocationCreateInfo& pAllocationCreateInfo, VkDeviceSize minAlignment, VkBuffer& pBuffer, VmaAllocation& pAllocation, VmaAllocationInfo& pAllocationInfo) {
		return vmaCreateBufferWithAlignment(m_allocator, &pBufferCreateInfo, &pAllocationCreateInfo, minAlignment, &pBuffer, &pAllocation, &pAllocationInfo); /// @todo
	}
	DEPRECATED VkResult createAliasingBuffer(const VmaAllocation& allocation, const VkBufferCreateInfo& pBufferCreateInfo, VkBuffer& pBuffer) {
		return vmaCreateAliasingBuffer(m_allocator, allocation, &pBufferCreateInfo, &pBuffer); /// @todo
	}
	// VkResult createAliasingBuffer2(const VmaAllocation& allocation, VkDeviceSize allocationLocalOffset, const VkBufferCreateInfo& pBufferCreateInfo, VkBuffer& pBuffer) {
	// 	vmaCreateAliasingBuffer2(m_allocator, allocation, allocationLocalOffset, &pBufferCreateInfo, &pBuffer);
	// }
	DEPRECATED VkResult createImage(const VkImageCreateInfo& pImageCreateInfo, const VmaAllocationCreateInfo& pAllocationCreateInfo, VkImage& pImage, VmaAllocation& pAllocation, VmaAllocationInfo pAllocationInfo = VmaAllocationInfo{}) {
		return vmaCreateImage(m_allocator, &pImageCreateInfo, &pAllocationCreateInfo, &pImage, &pAllocation, &pAllocationInfo);
	}
	DEPRECATED VkResult createAliasingImage(const VmaAllocation& allocation, const VkImageCreateInfo& pImageCreateInfo, VkImage& pImage) {
		return vmaCreateAliasingImage(m_allocator, allocation, &pImageCreateInfo, &pImage); /// @todo
	}
	// VkResult createAliasingImage2(const VmaAllocation& allocation, VkDeviceSize allocationLocalOffset, const VkImageCreateInfo& pImageCreateInfo, VkImage& pImage) {
	// 	vmaCreateAliasingImage2(m_allocator, allocationLocalOffset, &pImageCreateInfo, &pImage);
	// }
	DEPRECATED void destroyPool(const VmaPool& pool) {
		vmaDestroyPool(m_allocator, pool);
	}
	DEPRECATED void destroyBuffer(const VkBuffer& buffer, const VmaAllocation& allocation) {
		vmaDestroyBuffer(m_allocator, buffer, allocation);
	}
	DEPRECATED void destroyImage(const VkImage& image, const VmaAllocation& allocation) {
		vmaDestroyImage(m_allocator, image, allocation);
	}
	DEPRECATED void freeCommandBuffer(VkCommandPool commandPool, const VkCommandBuffer& pCommandBuffers) {
		vkFreeCommandBuffers(m_device, commandPool, 1, &pCommandBuffers);
	}
	DEPRECATED void freeCommandBuffers(VkCommandPool commandPool, const uint32& commandBufferCount, const VkCommandBuffer* pCommandBuffers) {
		vkFreeCommandBuffers(m_device, commandPool, commandBufferCount, pCommandBuffers);
	}
	VkResult freeDescriptorSets(const VkDescriptorPool& descriptorPool, const uint32& descriptorSetCount, const VkDescriptorSet& pDescriptorSets) {
		return vkFreeDescriptorSets(m_device, descriptorPool, descriptorSetCount, &pDescriptorSets);
	}
	VkResult findMemoryTypeIndexForBufferInfo(const VkBufferCreateInfo& pBufferCreateInfo, const VmaAllocationCreateInfo& pAllocationCreateInfo, uint32& pMemoryTypeIndex) {
		return vmaFindMemoryTypeIndexForBufferInfo(m_allocator, &pBufferCreateInfo, &pAllocationCreateInfo, &pMemoryTypeIndex);
	}
	VkResult findMemoryTypeIndexForImageInfo(const VkImageCreateInfo& pImageCreateInfo, const VmaAllocationCreateInfo& pAllocationCreateInfo, uint32& pMemoryTypeIndex) {
		return vmaFindMemoryTypeIndexForImageInfo(m_allocator, &pImageCreateInfo, &pAllocationCreateInfo, &pMemoryTypeIndex);
	}
	VkResult findMemoryTypeIndex(const uint32& memoryTypeBits, const VmaAllocationCreateInfo& pAllocationCreateInfo, uint32& pMemoryTypeIndex) {
		return vmaFindMemoryTypeIndex(m_allocator, memoryTypeBits, &pAllocationCreateInfo, &pMemoryTypeIndex);
	}
	VkResult checkPoolCorruption(const VmaPool& pool) {
		return vmaCheckPoolCorruption(m_allocator, pool);
	}
	void getPoolName(const VmaPool& pool, const char** ppName) {
		vmaGetPoolName(m_allocator, pool, ppName);
	}
	void setPoolName(const VmaPool& pool, const char* pName) {
		vmaSetPoolName(m_allocator, pool, pName);
	}
	VkResult allocateMemoryPages(const VkMemoryRequirements& pMemoryRequirements, const VmaAllocationCreateInfo& pCreateInfo, VmaAllocation& pAllocations, VmaAllocationInfo& pAllocationInfo) {
		return vmaAllocateMemoryPages(m_allocator, &pMemoryRequirements, &pCreateInfo, 1, &pAllocations, &pAllocationInfo);
	}
	VkResult allocateMemoryForBuffer(const VkBuffer& buffer, const VmaAllocationCreateInfo& pCreateInfo, VmaAllocation& pAllocation, VmaAllocationInfo& pAllocationInfo) {
		return vmaAllocateMemoryForBuffer(m_allocator, buffer, &pCreateInfo, &pAllocation, &pAllocationInfo);
	}
	VkResult allocateMemoryForImage(const VkImage& image, const VmaAllocationCreateInfo& pCreateInfo, VmaAllocation& pAllocation, VmaAllocationInfo& pAllocationInfo) {
		return vmaAllocateMemoryForImage(m_allocator, image, &pCreateInfo, &pAllocation, &pAllocationInfo);
	}
	void freeMemoryPages(const size_t& allocationCount, const VmaAllocation& pAllocations) {
		vmaFreeMemoryPages(m_allocator, allocationCount, &pAllocations);
	}
	void getAllocationInfo(const VmaAllocation& allocation, VmaAllocationInfo& pAllocationInfo) {
		vmaGetAllocationInfo(m_allocator, allocation, &pAllocationInfo);
	}
	void setAllocationUserData(const VmaAllocation& allocation, void* pUserData) {
		vmaSetAllocationUserData(m_allocator, allocation, pUserData);
	}
	void setAllocationName(const VmaAllocation& allocation, const char* pName) {
		vmaSetAllocationName(m_allocator, allocation, pName);
	}
	void getAllocationMemoryProperties(const VmaAllocation& allocation, VkMemoryPropertyFlags& pFlags) {
		vmaGetAllocationMemoryProperties(m_allocator, allocation, &pFlags);
	}
	VkResult flushAllocation(const VmaAllocation& allocation, VkDeviceSize offset, VkDeviceSize size) {
		return vmaFlushAllocation(m_allocator, allocation, offset, size);
	}
	VkResult invalidateAllocation(const VmaAllocation& allocation, VkDeviceSize offset, VkDeviceSize size) {
		return vmaInvalidateAllocation(m_allocator, allocation, offset, size);
	}
	VkResult flushAllocations(const uint32& allocationCount, const VmaAllocation& allocations, const VkDeviceSize& offsets, const VkDeviceSize& sizes) {
		return vmaFlushAllocations(m_allocator, allocationCount, &allocations, &offsets, &sizes);
	}
	VkResult invalidateAllocations(const uint32& allocationCount, const VmaAllocation& allocations, const VkDeviceSize& offsets, const VkDeviceSize& sizes) {
		return vmaInvalidateAllocations(m_allocator, allocationCount, &allocations, &offsets, &sizes);
	}
	VkResult checkCorruption(const uint32& memoryTypeBits) {
		return vmaCheckCorruption(m_allocator, memoryTypeBits);
	}
	VkResult beginDefragmentation(const VmaDefragmentationInfo& pInfo, VmaDefragmentationContext& pContext) {
		return vmaBeginDefragmentation(m_allocator, &pInfo, &pContext);
	}
	void endDefragmentation(const VmaDefragmentationContext&  context, VmaDefragmentationStats& pStats) {
		vmaEndDefragmentation(m_allocator, context, &pStats);
	}
	VkResult beginDefragmentationPass(const VmaDefragmentationContext&  context, VmaDefragmentationPassMoveInfo& pPassInfo) {
		return vmaBeginDefragmentationPass(m_allocator, context, &pPassInfo);
	}
	VkResult endDefragmentationPass(const VmaDefragmentationContext&  context, VmaDefragmentationPassMoveInfo& pPassInfo) {
		return vmaEndDefragmentationPass(m_allocator, context, &pPassInfo);
	}
	VkResult bindBufferMemory(const VmaAllocation& allocation, const VkBuffer& buffer) {
		return vmaBindBufferMemory(m_allocator, allocation, buffer);
	}
	VkResult bindBufferMemory2(const VmaAllocation& allocation, VkDeviceSize allocationLocalOffset, const VkBuffer& buffer, const void* pNext) {
		return vmaBindBufferMemory2(m_allocator, allocation, allocationLocalOffset, buffer, pNext);
	}
	VkResult bindImageMemory(const VmaAllocation& allocation, const VkImage& image) {
		return vmaBindImageMemory(m_allocator, allocation, image);
	}
	VkResult bindImageMemory2(const VmaAllocation& allocation, VkDeviceSize allocationLocalOffset, const VkImage& image, const void* pNext) {
		return vmaBindImageMemory2(m_allocator, allocation, allocationLocalOffset, image, pNext);
	}
	void freeMemory(const VmaAllocation& allocation) {
		vmaFreeMemory(m_allocator, allocation);
	}
	VkResult flushMappedMemoryRanges(const uint32& memoryRangeCount, const VkMappedMemoryRange& pMemoryRanges) {
		return vkFlushMappedMemoryRanges(m_device, memoryRangeCount, &pMemoryRanges);
	}
	VkResult getEventStatus(const VkEvent& event) {
		return vkGetEventStatus(m_device, event);
	}
	VkResult getFenceStatus(const VkFence& fence) {
		return vkGetFenceStatus(m_device, fence);
	}
	void getImageMemoryRequirements(const VkImage& image, VkMemoryRequirements& pMemoryRequirements) {
		vkGetImageMemoryRequirements(m_device, image, &pMemoryRequirements);
	}
	void getImageSparseMemoryRequirements(const VkImage& image, uint32& pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements& pSparseMemoryRequirements) {
		vkGetImageSparseMemoryRequirements(m_device, image, &pSparseMemoryRequirementCount, &pSparseMemoryRequirements);
	}
	void getImageSubresourceLayout(const VkImage& image, const VkImageSubresource& pSubresource, VkSubresourceLayout& pLayout) {
		vkGetImageSubresourceLayout(m_device, image, &pSubresource, &pLayout);
	}
	// physical device functions
	VkResult getPipelineCacheData(const VkPipelineCache& pipelineCache, size_t& pDataSize, void* pData) {
		return vkGetPipelineCacheData(m_device, pipelineCache, &pDataSize, pData);
	}
	VkResult getQueryPoolResults(const VkQueryPool& queryPool, const uint32& firstQuery, const uint32& queryCount, const size_t& dataSize, void* pData, const VkDeviceSize& stride, const VkQueryResultFlags& flags) {
		return vkGetQueryPoolResults(m_device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
	}
	void getRenderAreaGranularity(const VkRenderPass& renderPass, VkExtent2D& pGranularity) {
		vkGetRenderAreaGranularity(m_device, renderPass, &pGranularity);
	}
	VkResult invalidateMappedMemoryRanges(const uint32& memoryRangeCount, const VkMappedMemoryRange& pMemoryRanges) {
		return vkInvalidateMappedMemoryRanges(m_device, memoryRangeCount, &pMemoryRanges);
	}
	VkResult mapMemory(const VmaAllocation& allocation, void** ppData) {
		return vmaMapMemory(m_allocator, allocation, ppData);
	}
	VkResult mergePipelineCaches(const VkPipelineCache& dstCache, const uint32& srcCacheCount, const VkPipelineCache& pSrcCaches) {
		return vkMergePipelineCaches(m_device, dstCache, srcCacheCount, &pSrcCaches);
	}
	VkResult resetCommandPool(const VkCommandPool& commandPool, const VkCommandPoolResetFlags& flags) {
		return vkResetCommandPool(m_device, commandPool, flags);
	}
	VkResult resetDescriptorPool(const VkDescriptorPool& descriptorPool, const VkDescriptorPoolResetFlags& flags) {
		return vkResetDescriptorPool(m_device, descriptorPool, flags);
	}
	VkResult resetEvent(const VkEvent& event) {
		return vkResetEvent(m_device, event);
	}
	VkResult resetFence(const VkFence& pFence) {
		return vkResetFences(m_device, 1, &pFence);
	}
	VkResult resetFences(const uint32& fenceCount, const VkFence* pFences) {
		return vkResetFences(m_device, fenceCount, pFences);
	}
	VkResult setEvent(const VkEvent& event) {
		return vkSetEvent(m_device, event);
	}
	void unmapMemory(const vma::Allocation& allocation) {
		vmaUnmapMemory(m_allocator.get(), allocation);
	}
	void updateDescriptorSet(const VkWriteDescriptorSet& pDescriptorWrite, const VkCopyDescriptorSet& pDescriptorCopy) {
		vkUpdateDescriptorSets(m_device, 1, &pDescriptorWrite, 1, &pDescriptorCopy);
	}
	void updateDescriptorSets(const std::vector<VkWriteDescriptorSet>& descriptorWrites) {
		vkUpdateDescriptorSets(m_device, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
	void updateDescriptorSets(const std::vector<VkWriteDescriptorSet>& descriptorWrites, const std::vector<VkCopyDescriptorSet>& descriptorCopies) {
		vkUpdateDescriptorSets(m_device, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), static_cast<uint32>(descriptorCopies.size()), descriptorCopies.data());
	}
	VkResult waitForFence(const vk::Fence& pFences, const VkBool32& waitAll, uint64 timeout) {
		return vkWaitForFences(m_device, 1, &pFences.get(), waitAll, timeout);
	}
	VkResult waitForFences(const std::vector<VkFence>& fences, const VkBool32& waitAll, uint64 timeout) {
		return vkWaitForFences(m_device, static_cast<uint32>(fences.size()), fences.data(), waitAll, timeout);
	}

	// don't ask me how long this took

	/**
	 * @brief get the Vulkan instance
	 *
	 * @return const lyra::vulkan::vk::Instance&
	 */
	NODISCARD constexpr const vk::Instance& instance() const noexcept { return m_instance; }
	/**
	 * @brief get the GPU
	 *
	 * @return const lyra::vulkan::vk::PhysicalDevice&
	 */
	NODISCARD constexpr const vk::PhysicalDevice& physicalDevice() const noexcept { return m_physicalDevice;  }
	/**
	 * @brief get the logical device
	 * 
	 * @return const lyra::vulkan::vk::VkDevice&
	 */
	NODISCARD constexpr const vk::Device& device() const noexcept { return m_device; }
	/**
	 * @brief get the queue families of the device
	 * 
	 * @return NODISCARD constexpr lyra::vulkan::Device::QueueFamilies&
	 */
	NODISCARD constexpr const QueueFamilies& queueFamilies() const noexcept { return m_queueFamilies; }
	/**
	 * @brief get the graphics queue
	 * 
	 * @return const lyra::vulkan::vk::Queue&
	 */
	NODISCARD constexpr const vk::Queue& graphicsComputeQueue() const noexcept { return m_graphicsComputeQueue; }
	/**
	 * @brief get the presentation queue
	 *
	 * @return const lyra::vulkan::vk::Queue&
	 */
	NODISCARD constexpr const vk::Queue& presentQueue() const noexcept { return m_presentQueue; }
	/**
	 * @brief get the VMA memory allocator
	 *
	 * @return const VmaAllocator&
	 */
	NODISCARD constexpr const vma::Allocator& allocator() const noexcept { return m_allocator; }

private:
	vk::Instance m_instance;
	vk::PhysicalDevice m_physicalDevice;
	vk::Device m_device;

	QueueFamilies m_queueFamilies;
	vk::Queue m_graphicsComputeQueue;
	vk::Queue m_presentQueue;
	// vk::Queue m_computeQueue;

	vma::Allocator m_allocator;

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
	 * 
	 * @return true if all requested extensions were found
	 * @return false if not all requested extensions were found
	 */
	bool check_requested_extensions(const std::vector <VkExtensionProperties> extensions, const std::vector <const char*> requestedExtensions) const;

	/**
	 * @brief find the family index of a queues
	 *
	 * @param device device to find the family index of the queue of
	 * @param surface surface to find present queue compatibility
	 * 
	 * @return lyra::vulkan::Device::QueueFamily
	 */
	QueueFamilies find_queue_families(const VkPhysicalDevice& device, const vk::Surface& surface) noexcept;

	/**
	 * @brief rate a physical device by its features
	 *
	 * @param device the device to rate
	 * @param surface surface to find present queue compatibility
	 * @param map a map containing all the physical devices and their scores
	 */
	void rate_physical_device(const VkPhysicalDevice& device, const vk::Surface& surface, std::multimap <uint32, std::pair<VkPhysicalDevice, QueueFamilies>>& map);
	
	friend class RenderSystem;
};

} // namespace vulkan

} // namespace lyra
