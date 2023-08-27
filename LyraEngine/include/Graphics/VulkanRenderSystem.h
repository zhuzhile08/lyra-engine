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
#include <Common/Array.h>
#include <Common/Dynarray.h>
#include <Common/RAIIContainers.h>
#include <Common/Config.h>

#include <Graphics/ImGuiRenderer.h>

#include <glm/glm.hpp>
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

#include <variant>
#include <vector>
#include <unordered_set>

namespace lyra {

namespace vulkan {

namespace vk {

using Instance = RAIIContainer<VkInstance, NullHandle>;
using DebugUtilsMessengerEXT = RAIIContainer<VkDebugUtilsMessengerEXT, VkInstance>;
using DebugUtilsMessenger = DebugUtilsMessengerEXT;
using PhysicalDevice = RAIIContainer<VkPhysicalDevice, VkInstance>;
using Device = RAIIContainer<VkDevice, VkPhysicalDevice>;
using Queue = RAIIContainer<VkQueue, VkDevice>;
using SurfaceKHR = RAIIContainer<VkSurfaceKHR, VkInstance>;
using Surface = SurfaceKHR;
using CommandPool = RAIIContainer<VkCommandPool, VkDevice>;
using CommandBuffer = RAIIContainer<VkCommandBuffer, VkDevice>;
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
using DescriptorUpdateTemplate = RAIIContainer<VkDescriptorUpdateTemplate, VkDevice>;
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
	Array<uint32, 3> version;
	const Window* window;
};

class CommandQueue {
public:
	class CommandPool {
	public:
		CommandPool();

		void reset();

		vk::CommandPool commandPool;
	};

	class CommandBuffer {
	public:
		enum class Usage : uint32 {
			renderingDefault = 0x00000000,
			oneTimeSubmit = 0x00000001,
			renderPassContinue = 0x00000002,
			simultaneous = 0x00000004
		};

		CommandBuffer() noexcept = default;
		CommandBuffer(const CommandPool& commandPool, const VkCommandBufferLevel& level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		CommandBuffer(const VkCommandBuffer& commandBuffer) noexcept : 
			commandBuffer(vk::CommandBuffer(commandBuffer, VK_NULL_HANDLE)), // does not take ownership in this case
			commandPool(VK_NULL_HANDLE) { } 
		CommandBuffer(CommandBuffer&& movable) noexcept :
			commandBuffer(std::exchange(movable.commandBuffer, VkCommandBuffer { } )), 
			commandPool(std::exchange(movable.commandPool, VkCommandPool { } )) { }
		CommandBuffer& operator=(CommandBuffer&& movable) noexcept {
			if (&movable != this) {
				commandBuffer = std::exchange(movable.commandBuffer, VkCommandBuffer { } );
				commandPool = std::exchange(movable.commandPool, VkCommandPool { } );
			}
			return *this;
		}

		/**
		 * @brief wrappers around the core Vulkan API command functions
		 * @brief these are basically copied directly from the Vulkan API with minor modifications to reduce bloat and suit a more modern C++ style
		 * @brief please refer to the official Vulkan documentation (preferably at https:/devdocs.io// for the documentation of these functions
		 */

		void begin(const Usage& usage = Usage::renderingDefault) const;
		void end() const {
			VULKAN_ASSERT(vkEndCommandBuffer(commandBuffer), "stop recording command buffer");
		}
		void beginQuery(const VkQueryPool& queryPool, uint32 query, VkQueryControlFlags flags) const {
			vkCmdBeginQuery(commandBuffer, queryPool, query, flags);
		}
		void beginRenderPass(const VkRenderPassBeginInfo& renderPassBegin, const VkSubpassContents& contents) const {
			vkCmdBeginRenderPass(commandBuffer, &renderPassBegin, contents);
		}
		void bindDescriptorSet(
			VkPipelineBindPoint pipelineBindPoint,
			const vk::PipelineLayout& layout,
			uint32 firstSet,
			const vk::DescriptorSet& descriptorSet,
			uint32 dynamicOffset
		) const {
			vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, 1, &descriptorSet.get(), 1, &dynamicOffset);
		}
		void bindDescriptorSets(
			VkPipelineBindPoint pipelineBindPoint,
			const vk::PipelineLayout& layout,
			uint32 firstSet,
			const std::vector<VkDescriptorSet>& descriptorSets,
			const std::vector<uint32>& dynamicOffsets
		) const {
			vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSets.size(), descriptorSets.data(), dynamicOffsets.size(), dynamicOffsets.data());
		}
		void bindIndexBuffer(const vk::Buffer& buffer, VkDeviceSize offset, VkIndexType indexType) const {
			vkCmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
		}
		void bindPipeline(VkPipelineBindPoint pipelineBindPoint, const vk::Pipeline& pipeline) const {
			vkCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
		}
		void bindVertexBuffer(uint32 firstBinding, const vk::Buffer& buffer, VkDeviceSize offset) const {
			vkCmdBindVertexBuffers(commandBuffer, firstBinding, 1, &buffer.get(), &offset);
		}
		void bindVertexBuffers(uint32 firstBinding, const std::vector<VkBuffer>& buffers, const std::vector<VkDeviceSize>& offsets) const {
			vkCmdBindVertexBuffers(commandBuffer, firstBinding, buffers.size(), buffers.data(), offsets.data());
		}
		void blitImage(
			const vk::Image& srcImage, 
			VkImageLayout srcImageLayout, 
			const vk::Image& dstImage, 
			VkImageLayout dstImageLayout, 
			const VkImageBlit& region, 
			VkFilter filter
		) const {
			vkCmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, 1, &region, filter);
		}
		void blitImage(
			const vk::Image& srcImage, 
			VkImageLayout srcImageLayout, 
			const vk::Image& dstImage, 
			VkImageLayout dstImageLayout, 
			const std::vector<VkImageBlit>& regions, 
			VkFilter filter
		) const {
			vkCmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regions.size(), regions.data(), filter);
		}
		void clearAttachment(const VkClearAttachment& attachment, const VkClearRect& rect) const {
			vkCmdClearAttachments(commandBuffer, 1, &attachment, 1, &rect);
		}
		void clearAttachments(const std::vector<VkClearAttachment> attachments, const std::vector<VkClearRect>& rects) const {
			vkCmdClearAttachments(commandBuffer, attachments.size(), attachments.data(), rects.size(), rects.data());
		}
		void clearColorImage(
			const vk::Image& image, 
			VkImageLayout imageLayout, 
			const VkClearColorValue& color, 
			const VkImageSubresourceRange& range
		) const {
			vkCmdClearColorImage(commandBuffer, image, imageLayout, &color, 1, &range);
		}
		void clearColorImage(
			const vk::Image& image, 
			VkImageLayout imageLayout,
			const VkClearColorValue& color, 
			const std::vector<VkImageSubresourceRange>& ranges
		) const {
			vkCmdClearColorImage(commandBuffer, image, imageLayout, &color, ranges.size(), ranges.data());
		}
		void clearDepthStencilImage(
			const vk::Image& image, 
			VkImageLayout imageLayout, 
			const VkClearDepthStencilValue& depthStencil, 
			const VkImageSubresourceRange& range
		) const {
			vkCmdClearDepthStencilImage(commandBuffer, image, imageLayout, &depthStencil, 1, &range);
		}
		void clearDepthStencilImage(
			const vk::Image&image, 
			VkImageLayout imageLayout, 
			const VkClearDepthStencilValue& depthStencil,  
			const std::vector<VkImageSubresourceRange>& ranges
		) const {
			vkCmdClearDepthStencilImage(commandBuffer, image, imageLayout, &depthStencil, ranges.size(), ranges.data());
		}
		void copyBuffer(const vk::Buffer& srcBuffer, const vk::Buffer& dstBuffer, const VkBufferCopy& region) const {
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &region);
		}
		void copyBuffer(const vk::Buffer& srcBuffer, const vk::Buffer& dstBuffer, const std::vector<VkBufferCopy>& regions) const {
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regions.size(), regions.data());
		}
		void copyBufferToImage(
			const vk::Buffer& srcBuffer, 
			const vk::Image& dstImage, 
			VkImageLayout dstImageLayout, 
			const VkBufferImageCopy& region
		) const {
			vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, 1, &region);
		}
		void copyBufferToImage(
			const vk::Buffer& srcBuffer, 
			const vk::Image& dstImage, 
			VkImageLayout dstImageLayout, 
			const std::vector<VkBufferImageCopy>& regions
		) const {
			vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regions.size(), regions.data());
		}
		void copyImage(
			const vk::Image& srcImage, 
			VkImageLayout srcImageLayout, 
			const vk::Image& dstImage, 
			VkImageLayout dstImageLayout, 
			const VkImageCopy& region
		) const {
			vkCmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, 1, &region);
		}
		void copyImage(
			const vk::Image& srcImage, 
			VkImageLayout srcImageLayout, 
			const vk::Image& dstImage, 
			VkImageLayout dstImageLayout, 
			const std::vector<VkImageCopy>& regions
		) const {
			vkCmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regions.size(), regions.data());
		}
		void copyImageToBuffer(
			const vk::Image& srcImage, 
			VkImageLayout srcImageLayout, 
			const vk::Buffer& dstBuffer, 
			const VkBufferImageCopy& region
		) const {
			vkCmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, 1, &region);
		}
		void copyImageToBuffer(
			const vk::Image& srcImage, 
			VkImageLayout srcImageLayout, 
			const vk::Buffer& dstBuffer, 
			const std::vector<VkBufferImageCopy>& regions
		) const {
			vkCmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regions.size(), regions.data());
		}
		void copyQueryPoolResults(
			const VkQueryPool queryPool,
			uint32 firstQuery,
			uint32 queryCount,
			const vk::Buffer& dstBuffer,
			VkDeviceSize dstOffset,
			VkDeviceSize stride,
			const VkQueryResultFlags& flags) {
			vkCmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
		}
		void dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) const {
			vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
		}
		void dispatchIndirect(const vk::Buffer& buffer, VkDeviceSize offset) const {
			vkCmdDispatchIndirect(commandBuffer, buffer, offset);
		}
		void draw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) const {
			vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
		}
		void drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, int32 vertexOffset, uint32 firstInstance) const {
			vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
		}
		void drawIndexedIndirect(const vk::Buffer& buffer, VkDeviceSize offset, uint32 drawCount, uint32 stride) const {
			vkCmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
		}
		void drawIndirect(const vk::Buffer& buffer, VkDeviceSize offset, uint32 drawCount, uint32 stride) const {
			vkCmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
		}
		void endQuery(const VkQueryPool& queryPool, uint32 query) const {
			vkCmdEndQuery(commandBuffer, queryPool, query);
		}
		void endRenderPass() const {
			vkCmdEndRenderPass(commandBuffer);
		}
		void executeCommands(const vk::CommandBuffer& commandBuffer) const {
			vkCmdExecuteCommands(commandBuffer, 1, &commandBuffer.get());
		}
		void executeCommands(const std::vector<VkCommandBuffer>& commandBuffers) const {
			vkCmdExecuteCommands(commandBuffer, commandBuffers.size(), commandBuffers.data());
		}
		void fillBuffer(const vk::Buffer& dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32 data) const {
			vkCmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
		}
		void nextSubpass(VkSubpassContents contents) const {
			vkCmdNextSubpass(commandBuffer, contents);
		}
		void pipelineBarrier(
			VkPipelineStageFlags srcStageFlags,
			VkPipelineStageFlags dstStageFlags,
			VkDependencyFlags dependency,
			const VkMemoryBarrier& memory = VkMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM },
			const VkBufferMemoryBarrier& buffer = VkBufferMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM },
			const VkImageMemoryBarrier& image = VkImageMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM }
		) const {
			vkCmdPipelineBarrier(
				commandBuffer,
				srcStageFlags,
				dstStageFlags,
				dependency,
				(memory.sType == VK_STRUCTURE_TYPE_MAX_ENUM) ? 0 : 1,
				&memory,
				(buffer.sType == VK_STRUCTURE_TYPE_MAX_ENUM) ? 0 : 1,
				&buffer,
				(image.sType == VK_STRUCTURE_TYPE_MAX_ENUM) ? 0 : 1,
				&image
			);
		}
		void pipelineBarrier(
			VkPipelineStageFlags srcStageFlags,
			VkPipelineStageFlags dstStageFlags,
			VkDependencyFlags dependency,
			const std::vector<VkMemoryBarrier>& memory,
			const std::vector<VkBufferMemoryBarrier>& buffer,
			const std::vector<VkImageMemoryBarrier>& image
		) const {
			vkCmdPipelineBarrier(
				commandBuffer,
				srcStageFlags,
				dstStageFlags,
				dependency,
				memory.size(),
				memory.data(),
				buffer.size(),
				buffer.data(),
				image.size(),
				image.data()
			);
		}
		void pushConstants(
			const vk::PipelineLayout& layout, 
			VkShaderStageFlags stageFlags, 
			uint32 offset, 
			uint32 size, 
			const void* values
		) const {
			vkCmdPushConstants(commandBuffer, layout, stageFlags, offset, size, values);
		}
		void resetEvent(const vk::Event event, VkPipelineStageFlags stageMask) const {
			vkCmdResetEvent(commandBuffer, event, stageMask);
		}
		void resetQueryPool(const vk::QueryPool queryPool, uint32 firstQuery, uint32 queryCount) const {
			vkCmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
		}
		void resolveImage(
			const vk::Image& srcImage, 
			VkImageLayout srcImageLayout, 
			const vk::Image& dstImage,
			VkImageLayout dstImageLayout, 
			const VkImageResolve& region) const {
			vkCmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, 1, &region);
		}
		void resolveImage(
			const vk::Image& srcImage, 
			VkImageLayout srcImageLayout, 
			const vk::Image& dstImage, 
			VkImageLayout dstImageLayout, 
			const std::vector<VkImageResolve>& regions) const {
			vkCmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regions.size(), regions.data());
		}
		void setBlendConstants(const float32 blendConstants[4]) const {
			vkCmdSetBlendConstants(commandBuffer, blendConstants);
		}
		void setDepthBias(float32 depthBiasConstantFactor, float32 depthBiasClamp, float32 depthBiasSlopeFactor) const {
			vkCmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
		}
		void setDepthBounds(float32 minDepthBounds, float32 maxDepthBounds) const {
			vkCmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
		}
		void setEvent(const vk::Event& event, VkPipelineStageFlags stageMask) const {
			vkCmdSetEvent(commandBuffer, event, stageMask);
		}
		void setLineWidth(float32 lineWidth) const {
			vkCmdSetLineWidth(commandBuffer, lineWidth);
		}
		void setScissor(uint32 firstScissor, const VkRect2D& scissor) const {
			vkCmdSetScissor(commandBuffer, firstScissor, 1, &scissor);
		}
		void setScissor(uint32 firstScissor, const std::vector<VkRect2D>& scissors) const {
			vkCmdSetScissor(commandBuffer, firstScissor, scissors.size(), scissors.data());
		}
		void setStencilCompareMask(VkStencilFaceFlags faceMask, uint32 compareMask) const {
			vkCmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
		}
		void setStencilReference(VkStencilFaceFlags faceMask, uint32 reference) const {
			vkCmdSetStencilReference(commandBuffer, faceMask, reference);
		}
		void setStencilWriteMask(VkStencilFaceFlags faceMask, uint32 writeMask) const {
			vkCmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
		}
		void setViewport(uint32 firstViewport, const VkViewport& pViewport) const {
			vkCmdSetViewport(commandBuffer, firstViewport, 1, &pViewport);
		}
		void setViewport(uint32 firstViewport, const std::vector<VkViewport>& viewports) const {
			vkCmdSetViewport(commandBuffer, firstViewport, viewports.size(), viewports.data());
		}
		void updateBuffer(const vk::Buffer& dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* data) const {
			vkCmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, data);
		}
		void waitEvents(
			const vk::Event& event,
			VkPipelineStageFlags srcStageMask,
			VkPipelineStageFlags dstStageMask,
			const VkMemoryBarrier& pMemoryBarrier,
			const VkBufferMemoryBarrier& pBufferMemoryBarrier,
			const VkImageMemoryBarrier& pImageMemoryBarrier
		) const {
			vkCmdWaitEvents(
				commandBuffer,
				1,
				&event.get(),
				srcStageMask,
				dstStageMask,
				(pMemoryBarrier.sType == VK_STRUCTURE_TYPE_MAX_ENUM) ? 0 : 1,
				&pMemoryBarrier,
				(pBufferMemoryBarrier.sType == VK_STRUCTURE_TYPE_MAX_ENUM) ? 0 : 1,
				&pBufferMemoryBarrier,
				(pImageMemoryBarrier.sType == VK_STRUCTURE_TYPE_MAX_ENUM) ? 0 : 1,
				&pImageMemoryBarrier
			);
		}
		void waitEvents(
			const std::vector<VkEvent>& events,
			VkPipelineStageFlags srcStageMask,
			VkPipelineStageFlags dstStageMask,
			const std::vector<VkMemoryBarrier>& memory,
			const std::vector<VkBufferMemoryBarrier>& buffer,
			const std::vector<VkImageMemoryBarrier>& image
		) const {
			vkCmdWaitEvents(
				commandBuffer, 
				events.size(), 
				events.data(), 
				srcStageMask, 
				dstStageMask, 
				memory.size(),
				memory.data(),
				buffer.size(),
				buffer.data(),
				image.size(),
				image.data()
			);
		}
		void writeTimestamp(VkPipelineStageFlagBits pipelineStage, const vk::QueryPool& queryPool, uint32 query) const {
			vkCmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
		}
		void reset(VkCommandBufferResetFlags flags = 0) const;

		vk::CommandBuffer commandBuffer;
		VkCommandPool commandPool;
	};

	CommandQueue();

	void reset();
	void submit(VkFence fence, bool wait = false);

	void oneTimeBegin();
	void oneTimeSubmit();
	
	VkQueue queue;

	std::vector<VkSemaphore> waitSemaphores;
	std::vector<VkSemaphore> signalSemaphores;
	std::vector<VkPipelineStageFlags> pipelineStageFlags;

	const CommandBuffer* activeCommandBuffer;
	std::vector<CommandPool> commandPools;

	uint32 currentFrame = 0;
};

class GPUMemory {
public:
	void destroy() { memory.destroy(); }

	NODISCARD constexpr static VmaAllocationCreateInfo getAllocCreateInfo(VmaMemoryUsage usage, VkMemoryPropertyFlags requiredFlags = 0) noexcept {
		return {
			0,
			usage,
			requiredFlags,
			0,
			0,
			0,
			nullptr,
			0
		}; // the rest is absolutely useless
	}

	vma::Allocation memory;
};

class GPUBuffer : public GPUMemory {
public:
	constexpr GPUBuffer() = default;
	GPUBuffer(VkDeviceSize size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memUsage);
	DEFINE_DEFAULT_MOVE(GPUBuffer);

	void copy(const GPUBuffer& srcBuffer);
	void copyData(const void* src, size_t copySize = 0);
	void copyData(const void** src, uint32 arraySize, size_t elementSize = 0);

	NODISCARD constexpr VkDescriptorBufferInfo getDescriptorBufferInfo() const noexcept {
		return {
			buffer, 
			0, 
			size
		};
	}
	
	NODISCARD constexpr VkBufferMemoryBarrier getBufferMemoryBarrier(
		const VkAccessFlags srcAccessMask, 
		const VkAccessFlags dstAccessMask,
		const uint32 srcQueueFamily = VK_QUEUE_FAMILY_IGNORED,
		const uint32 dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
	) const noexcept {
		return {
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			nullptr,
			srcAccessMask,
			dstAccessMask,
			srcQueueFamily,
			dstQueueFamily,
			buffer,
			0,
			size
		};
	}

	vk::Buffer buffer;
	VkDeviceSize size = 0;
};

class Image {
public:
	void destroy() {
		image.destroy();
		view.destroy();
	}

	NODISCARD constexpr VkImageCreateInfo getImageCreateInfo(
		VkFormat format,
		const VkExtent3D& extent,
		VkImageUsageFlags usage,
		uint32 mipLevels = 1,
		VkImageType imageType = VK_IMAGE_TYPE_2D,
		uint32 arrayLayers = 1,
		VkImageCreateFlags flags = 0,
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
		VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL
	) noexcept {
		m_tiling = tiling;

		return {
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			nullptr,
			flags,
			imageType,
			format,
			extent,
			mipLevels,
			arrayLayers,
			samples,
			tiling,
			usage,
			VK_SHARING_MODE_EXCLUSIVE, /** @todo may come back to this area later */
			0,
			0,
			VK_IMAGE_LAYOUT_UNDEFINED
		};
	}

	void createView(
		VkFormat format,
		const VkImageSubresourceRange& subresourceRange,
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D,
		const VkComponentMapping& colorComponents = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY }
	);

	void transitionLayout(
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		VkFormat format,
		const VkImageSubresourceRange& subresourceRange
	) const;
	
	NODISCARD VkFormat static getBestFormat(const std::vector<VkFormat>& candidates, VkFormatFeatureFlags features, VkImageTiling tiling);

	NODISCARD constexpr VkImageMemoryBarrier getImageMemoryBarrier(
		VkAccessFlags srcAccessMask,
		VkAccessFlags dstAccessMask,
		VkImageLayout srcLayout,
		VkImageLayout dstLayout,
		const VkImageSubresourceRange& subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1 , 0, 1 },
		uint32 srcQueueFamily = VK_QUEUE_FAMILY_IGNORED,
		uint32 dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
	) const noexcept {
		return {
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			nullptr,
			srcAccessMask,
			dstAccessMask,
			srcLayout,
			dstLayout,
			srcQueueFamily,
			dstQueueFamily,
			image,
			subresourceRange
		};
	}

	void copyFromBuffer(const vulkan::GPUBuffer& stagingBuffer, const VkExtent3D& extent, uint32 layerCount = 1);

	vk::Image image;
	vk::ImageView view;

	VkImageTiling m_tiling = VK_IMAGE_TILING_MAX_ENUM;

	friend class Window;
};

class Swapchain {
public:
	Swapchain() = default;
	Swapchain(CommandQueue& commandQueue);
	void createSwapchain();
	void createAttachments();

	bool update(bool windowChanged = false); // returns if the Swapchain has updated or not

	bool aquire();
	void begin();
	bool present();

	vk::Surface surface;
	vk::Swapchain swapchain;
	vk::Swapchain oldSwapchain;

	uint32 presentFamilyIndex = std::numeric_limits<uint32>::max();
	vk::Queue presentQueue;

	VkFormat format;
	VkExtent2D extent;

	Dynarray<Image, config::maxSwapchainImages> images;
	
	Image colorImage;
	GPUMemory colorMem;
	VkSampleCountFlagBits maxMultisamples = VK_SAMPLE_COUNT_1_BIT;

	Image depthImage;
	GPUMemory depthMem;
	VkFormat depthBufferFormat;

	Array<vk::Semaphore, config::maxFramesInFlight> imageAquiredSemaphores;
	Array<vk::Semaphore, config::maxFramesInFlight> submitFinishedSemaphores;
	Array<vk::Fence, config::maxFramesInFlight> renderFinishedFences;

	std::unordered_set<Framebuffers*> framebuffers;

	uint32 imageIndex = 0;
	uint32 currentFrame = 0;

	bool lostSurface = false;
	bool invalidSwapchain = false;
	bool invalidAttachments = false;

	CommandQueue* commandQueue;
	CommandQueue::CommandBuffer commandBuffer;
};

class Framebuffers {
public:
	// construct a framebuffer in the engine default configuration
	Framebuffers();
	// @todo add a constructor with custom attachments
	// Framebuffers(const std::vector<Arrachment>& attachments);
	~Framebuffers();

	void begin() const;
	void end() const;

	void destroyFramebuffers() {
		for (auto& framebuffer : framebuffers) framebuffer.destroy();
	}
	void createFramebuffers();
	
	vk::RenderPass renderPass;
	Dynarray <vulkan::vk::Framebuffer, config::maxSwapchainImages> framebuffers;
};

class Shader {
public:
	static constexpr const char* const entry = "main"; // entry will always be main

	enum class Type { 
		vertex = 0x00000001,
		tessellationControl = 0x00000002,
		tessellationEvaluation = 0x00000004,
		geometry = 0x00000008,
		fragment = 0x00000010,
		graphics = 0x0000001F,
		all = 0x7FFFFFFF,
		compute = 0x00000020,
		rayGeneration = 0x00000100,
		anyHit = 0x00000200,
		closestHit = 0x00000400,
		miss = 0x00000800,
		intersection = 0x00001000,
		callable = 0x00002000,
		task = 0x00000040,
		mesh = 0x00000080
	}; // Refer to the API for the documentation of these enums

	Shader() = default;
	Shader(Type type, std::vector<char>&& source);
	Shader(Type type, const std::vector<char>& source);

	NODISCARD constexpr VkPipelineShaderStageCreateInfo getStageCreateInfo() const noexcept {
		return {
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			static_cast<VkShaderStageFlagBits>(type),
			module,
			entry,
			nullptr
		};
	}

	vk::ShaderModule module;
	std::vector<char> shaderSrc;

	Type type;
};

class DescriptorWriter {
public:
	enum Type {
		sampler = 0,
		imageSampler = 1,
		sampledImage = 2,
		storageImage = 3,
		texelBuffer = 4,
		texelStorageBuffer = 5,
		uniformBuffer = 6,
		storageBuffer = 7,
		dynamicUniformBuffer = 8,  
		dynamicStorageBuffer = 9,
		inputAttachment = 10,
		inlineUniformBlock = 1000138000,
		mutableValve = 1000351000,
	};

	// creation data for a single descriptor with both image and buffer information
	struct Data {
		// image info
		VkDescriptorImageInfo imageInfo;
		// buffer info
		VkDescriptorBufferInfo bufferInfo;
		// binding to bind these to
		uint16 binding;
		// type of shader to bind these to
		Type type;
	};

	// creation data for a single descriptor with only image information
	struct ImageOnlyData {
		// image info
		VkDescriptorImageInfo imageInfo;
		// binding to bind these to
		uint16 binding;
		// type of shader to bind these to
		Type type;
	};

	// creation data for a single descriptor with only buffer information
	struct BufferOnlyData {
		// buffer info
		VkDescriptorBufferInfo bufferInfo;
		// binding to bind these to
		uint16 binding;
		// type of shader to bind these to
		Type type;
	};

	constexpr void addWrites(const std::vector<ImageOnlyData>& newWrites) noexcept {
		for (const auto& newWrite : newWrites) {
			m_writes.push_back({
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				nullptr,
				newWrite.binding,
				0,
				1,
				static_cast<VkDescriptorType>(newWrite.type),
				&newWrite.imageInfo,
				nullptr,
				nullptr
			});
		}
	}
	constexpr void addWrites(const std::vector<BufferOnlyData>& newWrites) noexcept {
		for (const auto& newWrite : newWrites) {
			m_writes.push_back({
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				nullptr,
				newWrite.binding,
				0,
				1,
				static_cast<VkDescriptorType>(newWrite.type),
				nullptr,
				&newWrite.bufferInfo,
				nullptr
			});
		}
	}
	constexpr void addWrites(const std::vector<Data>& newWrites) noexcept {
		for (const auto& [image_info, buffer_info, binding, type] : newWrites) {
			m_writes.push_back({
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				nullptr,
				binding,
				0,
				1,
				static_cast<VkDescriptorType>(type),
				&image_info,
				&buffer_info,
				nullptr
				});
		}
	}

private:
	std::vector<VkWriteDescriptorSet> m_writes;
};

class GraphicsProgram {
public:
	class Binder {
	public:
		// binding data
		struct Binding {
			// descriptor type
			DescriptorWriter::Type type;
			// shader the binding is in
			Shader::Type shaderType;
			// binding number
			uint32 binding;
			// set number
			uint32 set = 0;
			// array size
			uint32 arraySize = 1;
			// uses VK_KHR_push_descriptors
			bool dynamic = false;
			// immutable samplers
			const std::vector<VkSampler>& immutableSamplers = { };
		};

		// push constant data
		struct PushConstant {
			// shader the push constant is in
			Shader::Type shaderType;
			// size of the push constant
			uint32 size;
			// offset
			uint32 offset = 0;
		};

		constexpr void addBinding(const Binding& binding) {
			if (m_bindings.size() <= binding.set) {
				m_bindings.push_back({});
				m_bindingFlags.push_back({});
				m_bindingFlagsCreateInfo.push_back({});
			}
			// add the new binding
			m_bindings[binding.set].push_back({
				binding.binding,
				static_cast<VkDescriptorType>(binding.type),
				binding.arraySize,
				static_cast<VkShaderStageFlags>(binding.shaderType),
				binding.immutableSamplers.data()
				});
			
			if (binding.dynamic) {
				m_bindingFlags[binding.set].push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);

				m_bindingFlagsCreateInfo[binding.set] = {
					VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
					nullptr,
					static_cast<uint32>(m_bindingFlags[binding.set].size()),
					m_bindingFlags[binding.set].data()
				}; // not very efficient, a lot of reassigning, though all types here are trivially copiable, still a @todo
			} else {
				m_bindingFlags[binding.set].push_back(0);
			}
		}
		constexpr void addBindings(const std::vector<Binding>& bindings) {
			for (const auto& binding : bindings) addBinding(binding);
		}

		constexpr void addPushConstant(const PushConstant& pushConstant) {
			m_pushConstants.push_back({
				static_cast<VkShaderStageFlags>(pushConstant.shaderType),
				pushConstant.offset,
				pushConstant.size
			});
		}
		constexpr void addPushConstants(const std::vector<PushConstant>& pushConstants) {
			for (const auto& pushConstant : pushConstants) addPushConstant(pushConstant);
		}

	private:
		Dynarray<std::vector<VkDescriptorSetLayoutBinding>, config::maxShaderSets> m_bindings;
		Dynarray<std::vector<VkDescriptorBindingFlags>, config::maxShaderSets> m_bindingFlags;
		Dynarray<VkDescriptorSetLayoutBindingFlagsCreateInfo, config::maxShaderSets> m_bindingFlagsCreateInfo;
		std::vector<VkPushConstantRange> m_pushConstants;

		friend class GraphicsProgram;
	};

	GraphicsProgram() = default;
	// Constructs a shader program in engine standard layout
	// You should prefer to write your shaders in the engine standard layout
	GraphicsProgram(const Shader& vertexShader, const Shader& fragmentShader);
	// Constructs a shader program with a custom layout
	GraphicsProgram(const Shader& vertexShader, const Shader& fragmentShader, const Binder& binder);

	const Shader* vertexShader;
	const Shader* fragmentShader;

	Dynarray<vk::DescriptorSetLayout, config::maxShaderSets> descriptorSetLayouts;
	vk::PipelineLayout pipelineLayout;
};

class GraphicsPipeline {
public:
	static constexpr VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	class Builder {
	public:
		struct GraphicsPipelineCreateInfo {
			VkVertexInputBindingDescription meshBindingDescription;
			Array<VkVertexInputAttributeDescription, 4> meshAttributeDescriptions;
			VkPipelineVertexInputStateCreateInfo vertexInputInfo;
			VkPipelineInputAssemblyStateCreateInfo inputAssembly;
			VkPipelineTessellationStateCreateInfo tesselation;
			std::variant<bool, VkViewport> viewport;
			std::variant<bool, VkRect2D> scissor;
			VkPipelineRasterizationStateCreateInfo rasterizer;
			VkPipelineMultisampleStateCreateInfo multisampling;
			VkPipelineDepthStencilStateCreateInfo depthStencilState;
			VkPipelineColorBlendAttachmentState colorBlendAttachment;
			VkPipelineColorBlendStateCreateInfo colorBlending;
		};

		struct Viewport {
			glm::vec2 extent;
			glm::vec2 offset = {0, 0};
			float32 minDepth = 0.0f;
			float32 maxDepth = 1.0f;
		};

		struct Scissor {
			glm::uvec2 extent;
			glm::ivec2 offset = {0, 0};
		};

		enum class ColorBlending {
			BLEND_ENABLE = 1U,
			BLEND_DISABLE = 0U
		};
		using Colourblending = ColorBlending;

		enum class Tessellation {
			TESSELLATION_ENABLE = 1U,
			TESSELLATION_DISABLE = 0U
		};

		enum class Multisampling {
			MULTISAMPLING_ENABLE = 1U,
			MULTISAMPLING_DISABLE = 0U
		};

		enum class RenderMode {
			MODE_FILL = 0,
			MODE_LINE = 1,
			MODE_POINT = 2
		};

		enum class Culling {
			CULLING_NONE = 0x00000000,
			CULLING_FRONT = 0x00000001,
			CULLING_BACK = 0x00000002,
			CULLING_ALL = 0x00000003
		};

		enum class PolygonFrontFace {
			FRONT_FACE_COUNTER_CLOCKWISE = 0,
			FRONT_FACE_CLOCKWISE = 1
		};

		Builder() noexcept = default;
		Builder(const Framebuffers& renderer);

		constexpr void enableSampleShading(float32 strength) noexcept {
			m_createInfo.multisampling.sampleShadingEnable = VK_TRUE;
			m_createInfo.multisampling.minSampleShading = strength;
		}
		constexpr void setCullingMode(Culling cullingMode) noexcept {
			m_createInfo.rasterizer.cullMode = static_cast<VkCullModeFlags>(cullingMode);
		}
		constexpr void setRenderMode(RenderMode renderMode) noexcept {
			m_createInfo.rasterizer.polygonMode = static_cast<VkPolygonMode>(renderMode);
		}
		constexpr void setPolyonFrontFace(PolygonFrontFace frontFace) noexcept {
			m_createInfo.rasterizer.frontFace = static_cast<VkFrontFace>(frontFace);
		}
		constexpr void setViewport(const Viewport& viewport) noexcept {
			m_createInfo.viewport = VkViewport {
				viewport.offset.x,
				viewport.offset.y,
				viewport.extent.x,
				viewport.extent.y,
				viewport.minDepth,
				viewport.maxDepth
			};
		}
		constexpr void setScissor(const Scissor& scissor) noexcept {
			m_createInfo.scissor = VkRect2D {
				{ scissor.offset.x, scissor.offset.y },
				{ scissor.extent.x, scissor.extent.y }
			};
		}

	private:
		GraphicsPipelineCreateInfo m_createInfo;
		const Framebuffers* m_renderer;

		friend class GraphicsPipeline;
	};

	GraphicsPipeline() noexcept = default;
	GraphicsPipeline(const GraphicsProgram& program, const Builder& builder);

	void bind();

	vk::Pipeline pipeline;
};

class ComputeProgram {
public:

};

class ComputePipeline {
public:

};

class DescriptorPools {
public:
	struct Size {
		DescriptorWriter::Type type;
		uint32 multiplier = 1;
	};

	enum class Flags {
		freeDescriptorSet = 0x00000001,
		updateAfterBind = 0x00000002,
		hostOnly = 0x00000004
	};

	DescriptorPools() = default;
	DescriptorPools(const std::vector<Size>& sizes, Flags flags = Flags::freeDescriptorSet);

	void reset();
	void allocAndBind(
		const CommandQueue::CommandBuffer& cmdBuff, 
		const DescriptorWriter& writer, 
		const GraphicsProgram& program,
		uint32 layoutIndex
	);

	std::vector<vk::DescriptorPool> descriptorPools;

	VkDescriptorPoolCreateInfo createInfo;
	std::vector<VkDescriptorPoolSize> sizes;
};

} // namespace vulkan

bool beginFrame();
void endFrame();

void initRenderSystem(const vulkan::InitInfo& info);
void quitRenderSystem();

class VulkanImGuiRenderer : public ImGuiRenderer {
public:
	VulkanImGuiRenderer() = default;
	VulkanImGuiRenderer(const Window& window);
	~VulkanImGuiRenderer();

	// Call this after adding all fonts
	void uploadFonts();

private:
	void beginFrame() final;
	void endFrame() final;

	vulkan::DescriptorPools m_descriptorPools;
	vulkan::Framebuffers m_framebuffers;
};

} // namespace lyra
