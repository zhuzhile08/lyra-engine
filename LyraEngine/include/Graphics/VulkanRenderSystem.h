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

#include <vector>
#include <string>

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
			USAGE_RENDERING_DEFAULT = 0x00000000,
			USAGE_ONE_TIME_SUBMIT = 0x00000001,
			USAGE_RENDER_PASS_CONTINUE = 0x00000002,
			USAGE_SIMULTANIOUS = 0x00000004
		};

		CommandBuffer() = default;
		CommandBuffer(const CommandPool& commandPool, const VkCommandBufferLevel& level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		CommandBuffer(const VkCommandBuffer& commandBuffer) : commandBuffer(vk::CommandBuffer(commandBuffer, VK_NULL_HANDLE)) { } // does not take ownership in this case
		CommandBuffer(CommandBuffer&& movable) : 
			commandBuffer(std::exchange(movable.commandBuffer, VkCommandBuffer { } )), 
			commandPool(std::exchange(movable.commandPool, VkCommandPool { } )) { }
		CommandBuffer& operator=(CommandBuffer&& movable) {
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

		void begin(const Usage& usage = Usage::USAGE_RENDERING_DEFAULT) const;
		void end() const {
			vassert(vkEndCommandBuffer(commandBuffer), "stop recording command buffer");
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
	
	uint32 queueFamily;
	VkQueue queue;

	std::vector<VkSemaphore> waitSemaphores;
	std::vector<VkSemaphore> signalSemaphores;
	std::vector<VkPipelineStageFlags> pipelineStageFlags;

	VkCommandBuffer activeCommandBuffer;
	std::vector<CommandPool> commandPools;

	uint32 currentFrame;
};

class GPUMemory {
public:
	void destroy() { memory.destroy(); }

	NODISCARD constexpr static VmaAllocationCreateInfo get_alloc_create_info(VmaMemoryUsage usage, VkMemoryPropertyFlags requiredFlags = 0) noexcept {
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
	void copy_data(const void* src, size_t copySize = 0);
	void copy_data(const void** src, uint32 arraySize, size_t elementSize = 0);

	NODISCARD constexpr VkDescriptorBufferInfo get_descriptor_buffer_info() const noexcept {
		return {
			buffer, 
			0, 
			size
		};
	}
	
	NODISCARD constexpr VkBufferMemoryBarrier get_buffer_memory_barrier(
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

	NODISCARD constexpr VkImageCreateInfo get_image_create_info(
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

	void create_view(
		VkFormat format,
		const VkImageSubresourceRange& subresourceRange,
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D,
		const VkComponentMapping& colorComponents = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY }
	);

	void transition_layout(
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		VkFormat format,
		const VkImageSubresourceRange& subresourceRange
	) const;
	
	NODISCARD VkFormat static get_best_format(const std::vector<VkFormat>& candidates, VkFormatFeatureFlags features, VkImageTiling tiling);

	NODISCARD constexpr VkImageMemoryBarrier get_image_memory_barrier(
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

	void copy_from_buffer(const vulkan::GPUBuffer& stagingBuffer, const VkExtent3D& extent, uint32 layerCount = 1);

	vk::Image image;
	vk::ImageView view;

	VkImageTiling m_tiling = VK_IMAGE_TILING_MAX_ENUM;

	friend class Window;
};

class Swapchain {
public:
	Swapchain() = default;
	Swapchain(SDL_Window* window, CommandQueue& commandQueue);
	void createSwapchain();
	void createAttachments();
	void update(bool windowModified);

	void aquire();
	void present();

	vk::Surface surface;
	vk::Swapchain swapchain;
	vk::Swapchain* oldSwapchain = nullptr;

	uint32 presentFamilyIndex = std::numeric_limits<uint32>::max();
	VkQueue presentQueue;

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

	bool lostSurface = false;
	bool invalidSwapchain = false;
	bool invalidAttachments = false;

	CommandQueue* commandQueue;
	SDL_Window* window;
};

class DescriptorSet {
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
		const VkDescriptorImageInfo& imageInfo;
		// buffer info
		const VkDescriptorBufferInfo& bufferInfo;
		// binding to bind these to
		const uint16& binding;
		// type of shader to bind these to
		const Type& type;
	};

	// creation data for a single descriptor with only image information
	struct ImageOnlyData {
		// image info
		const VkDescriptorImageInfo& imageInfo;
		// binding to bind these to
		const uint16& binding;
		// type of shader to bind these to
		const Type& type;
	};

	// creation data for a single descriptor with only buffer information
	struct BufferOnlyData {
		// buffer info
		const VkDescriptorBufferInfo& bufferInfo;
		// binding to bind these to
		const uint16& binding;
		// type of shader to bind these to
		const Type& type;
	};

	struct Binder {
		void add_writes(const std::vector<DescriptorSet::ImageOnlyData>& newWrites) noexcept;
		void add_writes(const std::vector<DescriptorSet::BufferOnlyData>& newWrites) noexcept;
		void add_writes(const std::vector<DescriptorSet::Data>& newWrites) noexcept;

		void update(vk::DescriptorSet& descriptorSet) noexcept;

		std::vector<VkWriteDescriptorSet> writes;
	};

	DescriptorSet() = default;
	DescriptorSet(const VkDescriptorSetAllocateInfo& allocInfo);

	vk::DescriptorSet descriptorSet;
};

/*

class DescriptorPoolSystem {
public:
	// simplified size data
	struct Size {
		// type of descriptor
		const uint32& type;
		// multiplier for the descriptor allocation count
		const uint32& multiplier;
	};

	class DescriptorPool {
	public:
		constexpr DescriptorPool() = default;
		DescriptorPool(const VkDescriptorPoolCreateInfo& createInfo);

		vk::DescriptorPool descriptorPool;
		size_t allocatedSets;
	};

	class PoolBuilder {
	public:
		PoolBuilder() = default;
		PoolBuilder(const PoolBuilder& poolBuilder)
			 : m_poolSizes(poolBuilder.m_poolSizes), m_poolFlags(poolBuilder.m_poolFlags) { } // , m_maxSets(poolBuilder.m_maxSets) { }

		void add_pool_size(const DescriptorPool::Size& newSize) noexcept {
			// add the size according to the type
			m_poolSizes.push_back({
				static_cast<VkDescriptorType>(newSize.type),
				uint32(newSize.multiplier * config::maxDescriptorTypePerPool)
			});

			// m_maxSets = config::maxDescriptorTypePerPool * newSize.multiplier;
		}
		void add_pool_sizes(const std::vector<DescriptorPool::Size>& newSizes) noexcept {
			// loop through all the types and create their respective sizes
			for (const auto& newSize : newSizes) add_pool_size(newSize);
		}

		void set_pool_flags(const VkDescriptorPoolCreateFlags& poolFlags) noexcept {
			m_poolFlags = poolFlags;
		}

		constexpr VkDescriptorPoolCreateInfo build_create_info() const  {
			// return the create info
			return VkDescriptorPoolCreateInfo {
				VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				nullptr,
				m_poolFlags,
				config::maxDescriptorTypePerPool,
				// m_maxSets,
				static_cast<uint32>(m_poolSizes.size()),
				m_poolSizes.data()
			};
		}

	private:
		std::vector<VkDescriptorPoolSize> m_poolSizes;
		VkDescriptorPoolCreateFlags m_poolFlags = 0;
		// uint32 m_maxSets;
	};

	DescriptorSystem() = default;
	DescriptorSystem(const LayoutBuilder& layoutBuilder, const PoolBuilder& poolBuilder) : m_poolBuilder(poolBuilder) {
		m_layout = DescriptorSetLayout(layoutBuilder.build_create_info());
	}
	

	NODISCARD DescriptorSetResource get_unused_set();

	std::vector<DescriptorPool> pools;
	ResourcePool<DescriptorSet> sets;

	PoolBuilder poolBuilder;

	void create_descriptor_pool();
};

*/

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
	Shader(std::vector<char>&& source);
	Shader(const std::vector<char>& source);

	NODISCARD constexpr VkPipelineShaderStageCreateInfo get_stage_create_info() const noexcept {
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

class Program {
public:
	struct Binding {
		DescriptorSet::Type type;
		uint32 set;
		uint32 binding;
		uint32 arraySize;
	};

	enum class Flags {
		shadingAmbient = 0x00010000,
		shadingDiffuse = 0x00020000,
		shadingSpecular = 0x00040000,
		shadingPhong = shadingAmbient | shadingDiffuse | shadingSpecular,
		shadingCel = 0x00080000,
		shadingCelHighDetail = 0x00100000,

		outline = 0x01000000,
		outlineFast = 0x02000000,

		armature = 0x10000000,

		universal = 0x7FFF0000
	};

	Program();

	const Shader* vertexShader;
	const Shader* fragmentShader;

	vk::DescriptorSetLayout descriptorSetLayout;
	vk::PipelineLayout pipelineLayout;
};

} // namespace vulkan

/*

struct RenderSystem {
public:
	RenderSystem() = default;
	RenderSystem(Window* const window) : Swapchain(window) { }

	void wait_device_queue(const vulkan::vk::Queue& queue) const;
	void draw();

	NODISCARD uint8 currentFrame() const noexcept { return m_currentFrame; }
	NODISCARD uint32 imageIndex() const noexcept { return m_imageIndex; }

	vulkan::Device device;
	Array<vulkan::CommandPool, 4> commandPools;
	vulkan::Window vulkanWindow;
	Array<vulkan::Frame, config::maxFramesInFlight> frames;

	Dynarray<Renderer*, config::maxConcurrentRenderers> m_renderers;
	Dynarray<UniquePointer<Renderer>, config::maxConcurrentRenderers * 2> m_deletedRenderers;

	uint8 m_currentFrame = 0;
	uint8 m_pastFrame;
	uint32 m_imageIndex;

	Window* window;

	void add_renderer(Renderer* const renderer);

	void present_device_queue();
	void submit_device_queue(VkPipelineStageFlags stageFlags) const;

	void update_frame_count() noexcept;
};

*/

} // namespace lyra
