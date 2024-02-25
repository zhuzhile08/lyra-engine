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
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>
#ifdef __APPLE__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-extension"
#pragma clang diagnostic ignored "-Wnullability-completeness"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#include <vk_mem_alloc.h>
#pragma clang diagnostic pop
#else
#include <vk_mem_alloc.h>
#endif

#include <variant>
#include <Common/Vector.h>
#include <unordered_map>
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
using DescriptorSet = RAIIContainer<VkDescriptorSet, VkDescriptorPool>;
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
			const vk::DescriptorSet& descriptorSet
		) const {
			vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, 1, &descriptorSet.get(), 0, nullptr);
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
			const Vector<VkDescriptorSet>& descriptorSets,
			const Vector<uint32>& dynamicOffsets
		) const {
			vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, static_cast<uint32>(descriptorSets.size()), descriptorSets.data(), static_cast<uint32>(dynamicOffsets.size()), dynamicOffsets.data());
		}
		void bindIndexBuffer(const vk::Buffer& buffer, VkDeviceSize offset, VkIndexType indexType = VK_INDEX_TYPE_UINT32) const {
			vkCmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
		}
		void bindPipeline(VkPipelineBindPoint pipelineBindPoint, const vk::Pipeline& pipeline) const {
			vkCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
		}
		void bindVertexBuffer(const vk::Buffer& buffer, VkDeviceSize offset, uint32 firstBinding) const {
			vkCmdBindVertexBuffers(commandBuffer, firstBinding, 1, &buffer.get(), &offset);
		}
		void bindVertexBuffers(const Vector<VkBuffer>& buffers, const Vector<VkDeviceSize>& offsets, uint32 firstBinding) const {
			vkCmdBindVertexBuffers(commandBuffer, firstBinding, static_cast<uint32>(buffers.size()), buffers.data(), offsets.data());
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
			const Vector<VkImageBlit>& regions, 
			VkFilter filter
		) const {
			vkCmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, static_cast<uint32>(regions.size()), regions.data(), filter);
		}
		void clearAttachment(const VkClearAttachment& attachment, const VkClearRect& rect) const {
			vkCmdClearAttachments(commandBuffer, 1, &attachment, 1, &rect);
		}
		void clearAttachments(const Vector<VkClearAttachment> attachments, const Vector<VkClearRect>& rects) const {
			vkCmdClearAttachments(commandBuffer, static_cast<uint32>(attachments.size()), attachments.data(), static_cast<uint32>(rects.size()), rects.data());
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
			const Vector<VkImageSubresourceRange>& ranges
		) const {
			vkCmdClearColorImage(commandBuffer, image, imageLayout, &color, static_cast<uint32>(ranges.size()), ranges.data());
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
			const Vector<VkImageSubresourceRange>& ranges
		) const {
			vkCmdClearDepthStencilImage(commandBuffer, image, imageLayout, &depthStencil, static_cast<uint32>(ranges.size()), ranges.data());
		}
		void copyBuffer(const vk::Buffer& srcBuffer, const vk::Buffer& dstBuffer, const VkBufferCopy& region) const {
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &region);
		}
		void copyBuffer(const vk::Buffer& srcBuffer, const vk::Buffer& dstBuffer, const Vector<VkBufferCopy>& regions) const {
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, static_cast<uint32>(regions.size()), regions.data());
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
			const Vector<VkBufferImageCopy>& regions
		) const {
			vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, static_cast<uint32>(regions.size()), regions.data());
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
			const Vector<VkImageCopy>& regions
		) const {
			vkCmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, static_cast<uint32>(regions.size()), regions.data());
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
			const Vector<VkBufferImageCopy>& regions
		) const {
			vkCmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, static_cast<uint32>(regions.size()), regions.data());
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
		void executeCommands(const vk::CommandBuffer& cmdBuffer) const {
			vkCmdExecuteCommands(commandBuffer, 1, &cmdBuffer.get());
		}
		void executeCommands(const Vector<VkCommandBuffer>& cmdBuffers) const {
			vkCmdExecuteCommands(commandBuffer, static_cast<uint32>(cmdBuffers.size()), cmdBuffers.data());
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
			const VkMemoryBarrier& memory = { },
			const VkBufferMemoryBarrier& buffer = { },
			const VkImageMemoryBarrier& image = { }
		) const {
			vkCmdPipelineBarrier(
				commandBuffer,
				srcStageFlags,
				dstStageFlags,
				dependency,
				(memory.sType != VK_STRUCTURE_TYPE_MEMORY_BARRIER) ? 0 : 1,
				(memory.sType != VK_STRUCTURE_TYPE_MEMORY_BARRIER) ? nullptr : &memory,
				(buffer.sType != VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER) ? 0 : 1,
				(buffer.sType != VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER) ? nullptr : &buffer,
				(image.sType != VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER) ? 0 : 1,
				(image.sType != VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER) ? nullptr : &image
			);
		}
		void pipelineBarrier(
			VkPipelineStageFlags srcStageFlags,
			VkPipelineStageFlags dstStageFlags,
			VkDependencyFlags dependency,
			const Vector<VkMemoryBarrier>& memory,
			const Vector<VkBufferMemoryBarrier>& buffer,
			const Vector<VkImageMemoryBarrier>& image
		) const {
			vkCmdPipelineBarrier(
				commandBuffer,
				srcStageFlags,
				dstStageFlags,
				dependency,
				static_cast<uint32>(memory.size()),
				memory.data(),
				static_cast<uint32>(buffer.size()),
				buffer.data(),
				static_cast<uint32>(image.size()),
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
			const Vector<VkImageResolve>& regions) const {
			vkCmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, static_cast<uint32>(regions.size()), regions.data());
		}
		void setBlendConstants(float32 blendConstants[4]) const {
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
		void setScissor(const VkRect2D& scissor) const {
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
		}
		void setScissors(const Vector<VkRect2D>& scissors, uint32 firstScissor = 0) const {
			vkCmdSetScissor(commandBuffer, firstScissor, static_cast<uint32>(scissors.size()), scissors.data());
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
		void setViewport(const VkViewport& pViewport) const {
			vkCmdSetViewport(commandBuffer, 0, 1, &pViewport);
		}
		void setViewport(const Vector<VkViewport>& viewports, uint32 firstViewport = 0) const {
			vkCmdSetViewport(commandBuffer, firstViewport, static_cast<uint32>(viewports.size()), viewports.data());
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
			const Vector<VkEvent>& events,
			VkPipelineStageFlags srcStageMask,
			VkPipelineStageFlags dstStageMask,
			const Vector<VkMemoryBarrier>& memory,
			const Vector<VkBufferMemoryBarrier>& buffer,
			const Vector<VkImageMemoryBarrier>& image
		) const {
			vkCmdWaitEvents(
				commandBuffer, 
				static_cast<uint32>(events.size()),
				events.data(),
				srcStageMask, 
				dstStageMask, 
				static_cast<uint32>(memory.size()),
				memory.data(),
				static_cast<uint32>(buffer.size()),
				buffer.data(),
				static_cast<uint32>(image.size()),
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
	
	VkQueue queue = VK_NULL_HANDLE;

	Vector<VkSemaphore> waitSemaphores;
	Vector<VkSemaphore> signalSemaphores;
	Vector<VkPipelineStageFlags> pipelineStageFlags;

	const CommandBuffer* activeCommandBuffer;
	Vector<CommandPool> commandPools;

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
	
	NODISCARD constexpr VkBufferMemoryBarrier bufferMemoryBarrier(
		const VkAccessFlags srcAccessMask, 
		const VkAccessFlags dstAccessMask,
		uint32 srcQueueFamily = VK_QUEUE_FAMILY_IGNORED,
		uint32 dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
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
	enum class Compare {
		never,
		less,
		equal,
		lessOrEqual,
		greater,
		notEqual,
		greaterOrEqual,
		always
	};

	enum class SampleCount {
		bit1 = 0x00000001,
		bit2 = 0x00000002,
		bit4 = 0x00000004,
		bit8 = 0x00000008,
		bit6 = 0x00000010,
		bit32 = 0x00000020,
		bit64 = 0x00000040
	};

	void destroy() {
		image.destroy();
		view.destroy();
	}

	NODISCARD constexpr VkImageCreateInfo imageCreateInfo(
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

	void createImage(
		const VkImageCreateInfo& info,
		const VmaAllocationCreateInfo& allocInfo,
		vma::Allocation& memory
	);

	void createView(
		VkFormat format,
		const VkImageSubresourceRange& subresourceRange,
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D,
		const VkComponentMapping& colorComponents = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY }
	);

	void transitionLayout(
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		const VkImageSubresourceRange& subresourceRange
	) const;

	void copyFromBuffer(const vulkan::GPUBuffer& stagingBuffer, const VkExtent3D& extent, uint32 layerCount = 1);

	NODISCARD static vk::Sampler createSampler(
		VkSamplerAddressMode addressModeU,
		VkSamplerAddressMode addressModeV,
		VkSamplerAddressMode addressModeW,
		VkBorderColor borderColor,
		float32 maxLod,
		float32 minLod = 0.0f,
		float32 mipLodBias = 0.0f,
		VkFilter magFilter = VK_FILTER_LINEAR,
		VkFilter minFilter = VK_FILTER_LINEAR,
		VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		VkCompareOp compareOp = VK_COMPARE_OP_NEVER,
		VkBool32 unnormalizedCoordinates = VK_FALSE
	) noexcept;
	
	NODISCARD static VkFormat bestFormat(const Vector<VkFormat>& candidates, VkFormatFeatureFlags features, VkImageTiling tiling);

	NODISCARD constexpr VkImageMemoryBarrier imageMemoryBarrier(
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

	NODISCARD VkFormatProperties formatProperties(VkFormat format = VK_FORMAT_R8G8B8A8_SRGB) const noexcept;

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

	uint32 imageIndex = 0;
	uint32 currentFrame = 0;

	bool lostSurface = false;
	bool invalidSwapchain = false;
	bool invalidAttachments = false;

	CommandQueue* commandQueue;
	CommandQueue::CommandBuffer commandBuffer;
};

class RenderTarget {
public:
	// construct a framebuffer in the engine default configuration
	RenderTarget();
	// @todo add a constructor with custom attachments
	// RenderTarget(const Vector<Arrachment>& attachments);
	~RenderTarget();

	void begin() const;
	void end() const;

	void destroyFramebuffers() {
		for (auto& target : framebuffers) target.destroy();
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
	Shader(Type type, Vector<char>&& source);
	Shader(Type type, const Vector<char>& source);

	NODISCARD constexpr VkPipelineShaderStageCreateInfo stageCreateInfo() const noexcept {
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
	Vector<char> shaderSrc;

	Type type;
};

class DescriptorSets {
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
		inputAttachment = 10
	};

	struct ImageWrite {
		Vector<VkDescriptorImageInfo> infos;
		uint32 binding;
		Type type;
	};

	struct BufferWrite {
		Vector<VkDescriptorBufferInfo> infos;
		uint32 binding;
		Type type;
	};
	
	DescriptorSets() = default;
	DescriptorSets(
		const GraphicsProgram& graphicsProgram, 
		uint32 layoutIndex,
		bool variableCount = false
	) : variableCount(variableCount), layoutIndex(layoutIndex), graphicsProgram(&graphicsProgram) { }
	~DescriptorSets();

	constexpr void addWrites(const Vector<ImageWrite>& newWrites) noexcept {
		dirty = true;

		imageWrites.reserve(imageWrites.size() + newWrites.size());
		imageWrites.insert(imageWrites.end(), newWrites.begin(), newWrites.end());
	}
	constexpr void addWrites(const Vector<BufferWrite>& newWrites) noexcept {
		dirty = true;

		bufferWrites.reserve(bufferWrites.size() + newWrites.size());
		bufferWrites.insert(bufferWrites.end(), newWrites.begin(), newWrites.end());
	}

	void update(uint32 index = std::numeric_limits<uint32>::max());

	void addDescriptorSets(uint32 count);

	void bind(uint32 index);

	Vector<vk::DescriptorSet> descriptorSets;

	Vector<ImageWrite> imageWrites;
	Vector<BufferWrite> bufferWrites;
	Vector<VkWriteDescriptorSet> writes;

	bool variableCount;

	bool dirty = false;

	uint32 layoutIndex;

	const GraphicsProgram* graphicsProgram;
};

class DescriptorPools {
public:
	struct Size {
		DescriptorSets::Type type;
		uint32 multiplier = 1;
	};

	enum class Flags {
		freeDescriptorSet = 0x00000001,
		updateAfterBind = 0x00000002,
		hostOnly = 0x00000004
	};

	DescriptorPools() = default;
	DescriptorPools(const Vector<Size>& sizes, Flags flags = Flags::freeDescriptorSet);

	void reset();
	vk::DescriptorSet allocate(const GraphicsProgram& program, uint32 layoutIndex, bool variableCount);

	Vector<vk::DescriptorPool> descriptorPools;

	uint32 allocationIndex = 0;

	VkDescriptorPoolCreateInfo createInfo;
	Vector<VkDescriptorPoolSize> sizes;
};

class GraphicsProgram {
public:
	class Builder {
	public:
		enum class Flags {
			updateAfterBind = 0x00000001,
			updateWhilePending = 0x00000002,
			partiallyBound = 0x00000004,
			variableCount = 0x00000008
		};

		struct Binding {
			// descriptor type
			DescriptorSets::Type type;
			// shader the binding is in
			Shader::Type shaderType;
			// set number
			uint32 set = 0;
			// array size
			uint32 arraySize = 1;
			// flags
			Flags flags;
			// immutable samplers
			const Vector<VkSampler>& immutableSamplers = { };
		};
		
		struct PushConstant {
			// shader the push constant is in
			Shader::Type shaderType;
			// size of the push constant
			uint32 size;
		};

		constexpr void addBinding(const Binding& binding) {
			if (m_bindings.size() <= binding.set) {
				m_bindings.pushBack({});
				m_bindingFlags.pushBack({});
				m_bindingFlagsCreateInfo.pushBack({});
			}

			// add the new binding
			m_bindings[binding.set].pushBack({
				static_cast<uint32>(m_bindings[binding.set].size()),
				static_cast<VkDescriptorType>(binding.type),
				binding.arraySize,
				static_cast<VkShaderStageFlags>(binding.shaderType),
				binding.immutableSamplers.data()
				});

			m_bindingFlags[binding.set].pushBack(static_cast<VkDescriptorBindingFlags>(binding.flags));
			
			if ((binding.flags | Flags::variableCount) == Flags::variableCount) {
				if (m_bindingFlagsCreateInfo.size() <= binding.set) m_bindingFlagsCreateInfo.resize(binding.set);

				m_bindingFlagsCreateInfo[binding.set] = {
					VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
					nullptr,
					static_cast<uint32>(m_bindingFlags[binding.set].size()),
					m_bindingFlags[binding.set].data()
				}; // not very efficient, a lot of reassigning, though all types here are trivially copiable, still a @todo
			}

			// compute the hash from the binding
			m_bindingHash.append(
				std::to_string(binding.type) + 
				std::to_string(binding.shaderType) + 
				std::to_string(binding.set) + 
				std::to_string(binding.arraySize) + 
				std::to_string(binding.flags) + 
				std::to_string(binding.immutableSamplers.size())
			);
		}
		constexpr void addBindings(const Vector<Binding>& bindings) {
			for (const auto& binding : bindings) addBinding(binding);
		}

		constexpr void addPushConstant(const PushConstant& pushConstant) {
			m_pushConstants.pushBack({
				static_cast<VkShaderStageFlags>(pushConstant.shaderType),
				m_pushConstants.back().size + m_pushConstants.back().offset,
				pushConstant.size
			});

			m_pushConstantHash.append(std::to_string(pushConstant.shaderType) + std::to_string(pushConstant.size));
		}
		constexpr void addPushConstants(const Vector<PushConstant>& pushConstants) {
			for (const auto& pushConstant : pushConstants) addPushConstant(pushConstant);
		}

		constexpr void setVertexShader(const Shader& shader) {
			m_vertexShader = &shader;
		}
		constexpr void setFragmentShader(const Shader& shader) {
			m_fragmentShader = &shader;
		}

		std::string hash() const noexcept;

	private:
		Dynarray<Vector<VkDescriptorSetLayoutBinding>, config::maxShaderSets> m_bindings;
		Dynarray<Vector<VkDescriptorBindingFlags>, config::maxShaderSets> m_bindingFlags;
		Dynarray<VkDescriptorSetLayoutBindingFlagsCreateInfo, config::maxShaderSets> m_bindingFlagsCreateInfo;
		Vector<VkPushConstantRange> m_pushConstants;

		const Shader* m_vertexShader;
		const Shader* m_fragmentShader;

		std::string m_bindingHash;
		std::string m_pushConstantHash;

		friend class GraphicsProgram;
	};

	// Constructs a shader program in engine standard layout
	// You should prefer to write your shaders in the engine standard layout
	GraphicsProgram();
	// Constructs a shader program with a custom layout
	GraphicsProgram(const Builder& builder);

	Dynarray<vk::DescriptorSetLayout, config::maxShaderSets> descriptorSetLayouts;
	Dynarray<uint32, config::maxShaderSets> dynamicDescriptorCounts;
	vk::PipelineLayout pipelineLayout;

	const Shader* vertexShader;
	const Shader* fragmentShader;

	std::string hash;
};

class GraphicsPipeline {
public:
	static constexpr VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

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

	class Builder {
	public:
		enum class Topology {
			pointList,
			lineList,
			lineStrip,
			triangleList,
			triangleStrip,
			triangleFan,
			lineListAdjacent,
			lineStripAdjacent,
			triangleListAdjacent,
			triangleStripAdjacent,
			patchList,
		};

		enum class RenderMode {
			fill,
			line,
			point
		};

		enum class Culling {
			none,
			front,
			back,
			all
		};

		enum class PolygonFrontFace {
			counterClockwise,
			clockwise
		};

		struct DepthStencil {
			bool write;
			Image::Compare compare;
		};

		constexpr Builder() noexcept : 
			m_viewport(false),
			m_scissor(false),
			m_topology(Topology::triangleList),
			m_renderMode(RenderMode::fill),
			m_culling(Culling::back),
			m_polyFrontFace(PolygonFrontFace::counterClockwise),	
			m_sampleCount(static_cast<Image::SampleCount>(0)),
			m_sampleShading(false),
			m_depthStencil(DepthStencil {
				true,
				Image::Compare::less
			}),
			m_blendAttachments({{	// configure color blending
				VK_FALSE,
				VK_BLEND_FACTOR_SRC_ALPHA,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_FACTOR_ZERO,
				VK_BLEND_OP_ADD,
				VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
			}}),
			m_renderTarget(nullptr),
			m_graphicsProgram(nullptr) { }

		constexpr void setTopology(Topology topology) noexcept {
			m_topology = topology;
		}
		constexpr void setCullingMode(Culling culling) noexcept {
			m_culling = culling;
		}
		constexpr void setRenderMode(RenderMode renderMode) noexcept {
			m_renderMode = renderMode;
		}
		constexpr void setPolyonFrontFace(PolygonFrontFace frontFace) noexcept {
			m_polyFrontFace = frontFace;
		}
		constexpr void enableSampleShading(float32 strength) noexcept {
			m_sampleShading = strength;
		}

		constexpr void setViewport(const Viewport& viewport) noexcept {
			m_viewport = VkViewport {
				viewport.offset.x,
				viewport.offset.y,
				viewport.extent.x,
				viewport.extent.y,
				viewport.minDepth,
				viewport.maxDepth
			};
		}

		constexpr void dynamicViewport() noexcept {
			m_viewport = false;
		}
		constexpr void setScissor(const Scissor& scissor) noexcept {
			m_scissor = VkRect2D {
				{ scissor.offset.x, scissor.offset.y },
				{ scissor.extent.x, scissor.extent.y }
			};
		}
		constexpr void dynamicScissor() noexcept {
			m_scissor = false;
		}

		constexpr void addBlendAttachment(const VkPipelineColorBlendAttachmentState& attachment) noexcept {
			m_blendAttachments.pushBack(attachment);
		}
		constexpr void addBlendAttachments(const Vector<VkPipelineColorBlendAttachmentState>& attachments) noexcept {
			m_blendAttachments.insert(m_blendAttachments.end(), attachments.begin(), attachments.end());
		}

		constexpr void setRenderTarget(const RenderTarget& renderTarget) {
			m_renderTarget = &renderTarget;
		}
		constexpr void setGraphicsProgram(const GraphicsProgram& graphicsProgram) {
			m_graphicsProgram = &graphicsProgram;
		}

		NODISCARD std::string hash() const noexcept;

	private:
		std::variant<bool, VkViewport> m_viewport;
		std::variant<bool, VkRect2D> m_scissor;
		Topology m_topology;
		RenderMode m_renderMode;
		Culling m_culling;
		PolygonFrontFace m_polyFrontFace;
		Image::SampleCount m_sampleCount;
		std::variant<bool, float32> m_sampleShading;
		std::variant<bool, DepthStencil> m_depthStencil;
		Vector<VkPipelineColorBlendAttachmentState> m_blendAttachments; // I have no idea what this does @todo

		const RenderTarget* m_renderTarget;
		const GraphicsProgram* m_graphicsProgram;

		friend class GraphicsPipeline;
	};

	// create a graphics pipeline with default engine properties
	GraphicsPipeline();
	// create a graphics pipeline with custom properties
	GraphicsPipeline(const Builder& builder);

	void bind() const;

	vk::Pipeline pipeline;

	std::variant<bool, VkViewport> dynamicViewport;
	std::variant<bool, VkRect2D> dynamicScissor;

	const RenderTarget* renderTarget;
	const GraphicsProgram* program;

	std::string hash;
};

class ComputeProgram {
public:

};

class ComputePipeline {
public:

};

class ImGuiRenderer : public lyra::ImGuiRenderer {
public:
	ImGuiRenderer() = default;
	ImGuiRenderer(const Window& window);
	~ImGuiRenderer();

	// Call this after adding all fonts
	void uploadFonts();

private:
	void beginFrame() final;
	void endFrame() final;

	vulkan::DescriptorPools m_descriptorPools;
	vulkan::RenderTarget m_renderTarget;
};

} // namespace vulkan

} // namespace lyra
