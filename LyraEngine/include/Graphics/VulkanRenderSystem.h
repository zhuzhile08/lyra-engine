/*************************
 * @file VulkanRenderSystem.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief The implementation for all vulkan backend structures in their raw forms
 * 
 * @date 2023-06-04
 * 
 * @copyright Copyright (c) 2023
 *************************/

#pragma once

#include <Common/Common.h>
#include <LSD/Array.h>
#include <LSD/Dynarray.h>
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
#include <LSD/Vector.h>
#include <LSD/UnorderedSparseMap.h>
#include <LSD/UnorderedSparseSet.h>

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

class GPUMemory {
public:
	enum class Access {
		none = 0x00000000,
		indirectCommandRead = 0x00000001,
		indexRead = 0x00000002,
		vertexAttributeRead = 0x00000004,
		uniformRead = 0x00000008,
		inputAttachmentRead = 0x00000010,
		shaderRead = 0x00000020,
		shaderWrite = 0x00000040,
		colorAttachmentRead = 0x00000080,
		colorAttachmentWrite = 0x00000100,
		depthStencilAttachmentRead = 0x00000200,
		depthStencilAttachmentWrite = 0x00000400,
		transferRead = 0x00000800,
		transferWrite = 0x00001000,
		hostRead = 0x00002000,
		hostWrite = 0x00004000,
		memoryRead = 0x00008000,
		memoryWrite = 0x00010000
	};

	enum class LoadMode {
		load = 0,
		clear = 1,
		dontCare = 2
	};

	enum class StoreMode {
		store = 0,
		dontCare = 1
	};

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
	void copyData(const void* src, size_type copySize = 0);
	void copyData(const void** src, uint32 arraySize, size_type elementSize = 0);

	NODISCARD constexpr VkDescriptorBufferInfo getDescriptorBufferInfo() const noexcept {
		return {
			buffer, 
			0, 
			size
		};
	}
	
	NODISCARD constexpr VkBufferMemoryBarrier bufferMemoryBarrier(
		GPUMemory::Access srcAccessMask, 
		GPUMemory::Access dstAccessMask,
		uint32 srcQueueFamily = VK_QUEUE_FAMILY_IGNORED,
		uint32 dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
	) const noexcept {
		return {
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			nullptr,
			static_cast<VkAccessFlags>(srcAccessMask),
			static_cast<VkAccessFlags>(dstAccessMask),
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
	enum class Type { // also contains enums for VkImageViewType
		dim1,
		dim2,
		dim3,
		cube,
		arrayDim1,
		arrayDim2,
		arrayCube,
		max = 0x7FFFFFFF
	};

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

	enum class Aspect {
		none = 0,
		color = 0x00000001,
		depth = 0x00000002,
		stencil = 0x00000004,
		metadata = 0x00000008,
		plane0 = 0x00000010,
		plane1 = 0x00000020,
		plane2 = 0x00000040,
		max = 0x7FFFFFFF
	};

	enum class SampleCount {
		bit0 = 0x00000000, // not valid bit, only for utility
		bit1 = 0x00000001,
		bit2 = 0x00000002,
		bit4 = 0x00000004,
		bit8 = 0x00000008,
		bit6 = 0x00000010,
		bit32 = 0x00000020,
		bit64 = 0x00000040
	};

	enum class Layout {
		undefined,
		general,
		colorAttachment,
		depthStencilAttachment,
		depthStencilReadOnly,
		shaderReadOnly,
		transferSrc,
		transferDst,
		reinitialized,
		present = 1000001002,
		max = 0x7FFFFFFF
	};

	enum class Tiling {
		optimal,
		linear,
		max = 0x7FFFFFFF
	};

	enum class Format {
		undefined,
		r4g4UNormPack8,
		r4g4b4a4UNormPack16,
		b4g4r4a4UNormPack16,
		r5g6b5UNormPack16,
		b5g6r5UNormPack16,
		r5g5b5a1UNormPack16,
		b5g5r5a1UNormPack16,
		a1r5g5b5UNormPack16,
		r8UNorm,
		r8SNorm,
		r8UScaled,
		r8SScaled,
		r8UInt,
		r8SInt,
		r8SRGB,
		r8g8UNorm,
		r8g8SNorm,
		r8g8UScaled,
		r8g8SScaled,
		r8g8UInt,
		r8g8SInt,
		r8g8SRGB,
		r8g8b8UNorm,
		r8g8b8SNorm,
		r8g8b8UScaled,
		r8g8b8SScaled,
		r8g8b8UInt,
		r8g8b8SInt,
		r8g8b8SRGB,
		b8g8r8UNorm,
		b8g8r8SNorm,
		b8g8r8UScaled,
		b8g8r8SScaled,
		b8g8r8UInt,
		b8g8r8SInt,
		b8g8r8SRGB,
		r8g8b8a8UNorm,
		r8g8b8a8SNorm,
		r8g8b8a8UScaled,
		r8g8b8a8SScaled,
		r8g8b8a8UInt,
		r8g8b8a8SInt,
		r8g8b8a8SRGB,
		b8g8r8a8UNorm,
		b8g8r8a8SNorm,
		b8g8r8a8UScaled,
		b8g8r8a8SScaled,
		b8g8r8a8UInt,
		b8g8r8a8SInt,
		b8g8r8a8SRGB,
		a8b8g8r8UNormPack32,
		a8b8g8r8SNormPack32,
		a8b8g8r8UScaledPack32,
		a8b8g8r8SScaledPack32,
		a8b8g8r8UIntPack32,
		a8b8g8r8SIntPack32,
		a8b8g8r8SRGBPack32,
		a2r10g10b10UNormPack32,
		a2r10g10b10SNormPack32,
		a2r10g10b10UScaledPack32,
		a2r10g10b10SScaledPack32,
		a2r10g10b10UIntPack32,
		a2r10g10b10SIntPack32,
		a2b10g10r10UNormPack32,
		a2b10g10r10SNormPack32,
		a2b10g10r10UScaledPack32,
		a2b10g10r10SScaledPack32,
		a2b10g10r10UIntPack32,
		a2b10g10r10SIntPack32,
		r16UNorm,
		r16SNorm,
		r16UScaled,
		r16SScaled,
		r16UInt,
		r16SInt,
		r16SFloat,
		r16g16UNorm,
		r16g16SNorm,
		r16g16UScaled,
		r16g16SScaled,
		r16g16UInt,
		r16g16SInt,
		r16g16SFloat,
		r16g16b16UNorm,
		r16g16b16SNorm,
		r16g16b16UScaled,
		r16g16b16SScaled,
		r16g16b16UInt,
		r16g16b16SInt,
		r16g16b16SFloat,
		r16g16b16a16UNorm,
		r16g16b16a16SNorm,
		r16g16b16a16UScaled,
		r16g16b16a16SScaled,
		r16g16b16a16UInt,
		r16g16b16a16SInt,
		r16g16b16a16SFloat,
		r32UInt,
		r32SInt,
		r32SFloat,
		r32g32UInt,
		r32g32SInt,
		r32g32SFloat,
		r32g32b32UInt,
		r32g32b32SInt,
		r32g32b32SFloat,
		r32g32b32a32UInt,
		r32g32b32a32SInt,
		r32g32b32a32SFloat,
		r64UInt,
		r64SInt,
		r64SFloat,
		r64g64UInt,
		r64g64SInt,
		r64g64SFloat,
		r64g64b64UInt,
		r64g64b64SInt,
		r64g64b64SFloat,
		r64g64b64a64UInt,
		r64g64b64a64SInt,
		r64g64b64a64SFloat,
		b10g11r11UFloatPack32,
		e5b9g9r9UFloatPack32,
		d16UNorm,
		x8d24UNormPack32,
		d32SFloat,
		s8UInt,
		d16UNormS8UInt,
		d24UNormS8UInt,
		d32SFloatS8UInt,
		max = 0x7FFFFFFF
	};

	class Resource {
	public:
		Resource() = default;
		Resource(
			const Image& image,
			const VkImageSubresourceRange& subresourceRange,
			Type viewType = Type::dim2,
			Format format = Format::max,
			const VkComponentMapping& colorComponents = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY }
		);
	
		vk::ImageView view;
		const Image* image;
	};

	Image() = default;
	Image(const VkImageCreateInfo& info, const VmaAllocationCreateInfo& allocInfo, vma::Allocation& memory);

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

	NODISCARD static constexpr VkImageCreateInfo imageCreateInfo(
		Format format,
		const VkExtent3D& extent,
		VkImageUsageFlags usage,
		uint32 mipLevels = 1,
		Type imageType = Type::dim2,
		uint32 arrayLayers = 1,
		VkImageCreateFlags flags = 0,
		SampleCount samples = SampleCount::bit1,
		Tiling tiling = Tiling::optimal
	) noexcept {
		return {
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			nullptr,
			flags,
			static_cast<VkImageType>(imageType),
			static_cast<VkFormat>(format),
			extent,
			mipLevels,
			arrayLayers,
			static_cast<VkSampleCountFlagBits>(samples),
			static_cast<VkImageTiling>(tiling),
			usage,
			VK_SHARING_MODE_EXCLUSIVE, /** @todo may come back to this area later */
			0,
			0,
			VK_IMAGE_LAYOUT_UNDEFINED
		};
	}

	NODISCARD constexpr VkImageMemoryBarrier imageMemoryBarrier(
		GPUMemory::Access srcAccessMask,
		GPUMemory::Access dstAccessMask,
		Layout srcLayout,
		Layout dstLayout,
		const VkImageSubresourceRange& subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
		uint32 srcQueueFamily = VK_QUEUE_FAMILY_IGNORED,
		uint32 dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
	) const noexcept {
		return {
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			nullptr,
			static_cast<VkAccessFlags>(srcAccessMask),
			static_cast<VkAccessFlags>(dstAccessMask),
			static_cast<VkImageLayout>(srcLayout),
			static_cast<VkImageLayout>(dstLayout),
			srcQueueFamily,
			dstQueueFamily,
			image,
			subresourceRange
		};
	}
	
	NODISCARD static Format bestFormat(const lsd::Vector<Format>& candidates, Tiling tiling, VkFormatFeatureFlags features);
	NODISCARD VkFormatProperties formatProperties(Format format = Format::r8g8b8a8SRGB) const noexcept;

	void transitionLayout(Layout oldLayout, Layout newLayout, const VkImageSubresourceRange& subresourceRange) const;

	void copyFromBuffer(const vulkan::GPUBuffer& stagingBuffer, const VkExtent3D& extent, uint32 layerCount = 1);

	vk::Image image;

	Format format = Format::max;
	SampleCount samples = SampleCount::bit0;
	Tiling tiling = Tiling::max;

	friend class Window;
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
			const lsd::Vector<VkDescriptorSet>& descriptorSets,
			const lsd::Vector<uint32>& dynamicOffsets
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
		void bindVertexBuffers(const lsd::Vector<VkBuffer>& buffers, const lsd::Vector<VkDeviceSize>& offsets, uint32 firstBinding) const {
			vkCmdBindVertexBuffers(commandBuffer, firstBinding, static_cast<uint32>(buffers.size()), buffers.data(), offsets.data());
		}
		void blitImage(
			const vk::Image& srcImage, 
			Image::Layout srcImageLayout, 
			const vk::Image& dstImage, 
			Image::Layout dstImageLayout, 
			const VkImageBlit& region, 
			VkFilter filter
		) const {
			vkCmdBlitImage(commandBuffer, srcImage, static_cast<VkImageLayout>(srcImageLayout), dstImage, static_cast<VkImageLayout>(dstImageLayout), 1, &region, filter);
		}
		void blitImage(
			const vk::Image& srcImage, 
			Image::Layout srcImageLayout, 
			const vk::Image& dstImage, 
			Image::Layout dstImageLayout, 
			const lsd::Vector<VkImageBlit>& regions, 
			VkFilter filter
		) const {
			vkCmdBlitImage(commandBuffer, srcImage, static_cast<VkImageLayout>(srcImageLayout), dstImage, static_cast<VkImageLayout>(dstImageLayout), static_cast<uint32>(regions.size()), regions.data(), filter);
		}
		void clearAttachment(const VkClearAttachment& attachment, const VkClearRect& rect) const {
			vkCmdClearAttachments(commandBuffer, 1, &attachment, 1, &rect);
		}
		void clearAttachments(const lsd::Vector<VkClearAttachment> attachments, const lsd::Vector<VkClearRect>& rects) const {
			vkCmdClearAttachments(commandBuffer, static_cast<uint32>(attachments.size()), attachments.data(), static_cast<uint32>(rects.size()), rects.data());
		}
		void clearColorImage(
			const vk::Image& image, 
			Image::Layout imageLayout, 
			const VkClearColorValue& color, 
			const VkImageSubresourceRange& range
		) const {
			vkCmdClearColorImage(commandBuffer, image, static_cast<VkImageLayout>(imageLayout), &color, 1, &range);
		}
		void clearColorImage(
			const vk::Image& image, 
			Image::Layout imageLayout,
			const VkClearColorValue& color, 
			const lsd::Vector<VkImageSubresourceRange>& ranges
		) const {
			vkCmdClearColorImage(commandBuffer, image, static_cast<VkImageLayout>(imageLayout), &color, static_cast<uint32>(ranges.size()), ranges.data());
		}
		void clearDepthStencilImage(
			const vk::Image& image, 
			Image::Layout imageLayout, 
			const VkClearDepthStencilValue& depthStencil, 
			const VkImageSubresourceRange& range
		) const {
			vkCmdClearDepthStencilImage(commandBuffer, image, static_cast<VkImageLayout>(imageLayout), &depthStencil, 1, &range);
		}
		void clearDepthStencilImage(
			const vk::Image&image, 
			Image::Layout imageLayout, 
			const VkClearDepthStencilValue& depthStencil, 
			const lsd::Vector<VkImageSubresourceRange>& ranges
		) const {
			vkCmdClearDepthStencilImage(commandBuffer, image, static_cast<VkImageLayout>(imageLayout), &depthStencil, static_cast<uint32>(ranges.size()), ranges.data());
		}
		void copyBuffer(const vk::Buffer& srcBuffer, const vk::Buffer& dstBuffer, const VkBufferCopy& region) const {
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &region);
		}
		void copyBuffer(const vk::Buffer& srcBuffer, const vk::Buffer& dstBuffer, const lsd::Vector<VkBufferCopy>& regions) const {
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, static_cast<uint32>(regions.size()), regions.data());
		}
		void copyBufferToImage(
			const vk::Buffer& srcBuffer, 
			const vk::Image& dstImage, 
			Image::Layout dstImageLayout, 
			const VkBufferImageCopy& region
		) const {
			vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, static_cast<VkImageLayout>(dstImageLayout), 1, &region);
		}
		void copyBufferToImage(
			const vk::Buffer& srcBuffer, 
			const vk::Image& dstImage, 
			Image::Layout dstImageLayout, 
			const lsd::Vector<VkBufferImageCopy>& regions
		) const {
			vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, static_cast<VkImageLayout>(dstImageLayout), static_cast<uint32>(regions.size()), regions.data());
		}
		void copyImage(
			const vk::Image& srcImage, 
			Image::Layout srcImageLayout, 
			const vk::Image& dstImage, 
			Image::Layout dstImageLayout, 
			const VkImageCopy& region
		) const {
			vkCmdCopyImage(commandBuffer, srcImage, static_cast<VkImageLayout>(srcImageLayout), dstImage, static_cast<VkImageLayout>(dstImageLayout), 1, &region);
		}
		void copyImage(
			const vk::Image& srcImage, 
			Image::Layout srcImageLayout, 
			const vk::Image& dstImage, 
			Image::Layout dstImageLayout, 
			const lsd::Vector<VkImageCopy>& regions
		) const {
			vkCmdCopyImage(commandBuffer, srcImage, static_cast<VkImageLayout>(srcImageLayout), dstImage, static_cast<VkImageLayout>(dstImageLayout), static_cast<uint32>(regions.size()), regions.data());
		}
		void copyImageToBuffer(
			const vk::Image& srcImage, 
			Image::Layout srcImageLayout, 
			const vk::Buffer& dstBuffer, 
			const VkBufferImageCopy& region
		) const {
			vkCmdCopyImageToBuffer(commandBuffer, srcImage, static_cast<VkImageLayout>(srcImageLayout), dstBuffer, 1, &region);
		}
		void copyImageToBuffer(
			const vk::Image& srcImage, 
			Image::Layout srcImageLayout, 
			const vk::Buffer& dstBuffer, 
			const lsd::Vector<VkBufferImageCopy>& regions
		) const {
			vkCmdCopyImageToBuffer(commandBuffer, srcImage, static_cast<VkImageLayout>(srcImageLayout), dstBuffer, static_cast<uint32>(regions.size()), regions.data());
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
		void executeCommands(const lsd::Vector<VkCommandBuffer>& cmdBuffers) const {
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
			const lsd::Vector<VkMemoryBarrier>& memory,
			const lsd::Vector<VkBufferMemoryBarrier>& buffer,
			const lsd::Vector<VkImageMemoryBarrier>& image
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
			Image::Layout srcImageLayout, 
			const vk::Image& dstImage,
			Image::Layout dstImageLayout, 
			const VkImageResolve& region) const {
			vkCmdResolveImage(commandBuffer, srcImage, static_cast<VkImageLayout>(srcImageLayout), dstImage, static_cast<VkImageLayout>(dstImageLayout), 1, &region);
		}
		void resolveImage(
			const vk::Image& srcImage, 
			Image::Layout srcImageLayout, 
			const vk::Image& dstImage, 
			Image::Layout dstImageLayout, 
			const lsd::Vector<VkImageResolve>& regions) const {
			vkCmdResolveImage(commandBuffer, srcImage, static_cast<VkImageLayout>(srcImageLayout), dstImage, static_cast<VkImageLayout>(dstImageLayout), static_cast<uint32>(regions.size()), regions.data());
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
		void setScissors(const lsd::Vector<VkRect2D>& scissors, uint32 firstScissor = 0) const {
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
		void setViewport(const lsd::Vector<VkViewport>& viewports, uint32 firstViewport = 0) const {
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
			const lsd::Vector<VkEvent>& events,
			VkPipelineStageFlags srcStageMask,
			VkPipelineStageFlags dstStageMask,
			const lsd::Vector<VkMemoryBarrier>& memory,
			const lsd::Vector<VkBufferMemoryBarrier>& buffer,
			const lsd::Vector<VkImageMemoryBarrier>& image
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

	lsd::Vector<VkSemaphore> waitSemaphores;
	lsd::Vector<VkSemaphore> signalSemaphores;
	lsd::Vector<VkPipelineStageFlags> pipelineStageFlags;

	const CommandBuffer* activeCommandBuffer;
	lsd::Vector<CommandPool> commandPools;

	uint32 currentFrame = 0;
};

class Pipeline {
public:
	enum class BindPoint {
		graphics,
		compute
	};

	enum class Stage {
		none = 0x00000000,
		topOfPipe = 0x00000001,
		drawIndirect = 0x00000002,
		vertexInput = 0x00000004,
		vertexShader = 0x00000008,
		tessellationControlShader = 0x00000010,
		tessellationEvaluationShader = 0x00000020,
		geometryShader = 0x00000040,
		fragmentShader = 0x00000080,
		earlyFragmentTests = 0x00000100,
		lateFragmentTests = 0x00000200,
		colorAttachmentOutput = 0x00000400,
		computeShader = 0x00000800,
		transfer = 0x00001000,
		bottomOfPipe = 0x00002000,
		host = 0x00004000,
		allGraphics = 0x00008000,
		allCommands = 0x00010000,
	};

	Pipeline() = default;
	Pipeline(const RenderTarget* const renderTarget, const lsd::String& hash) : renderTarget(renderTarget), hash(hash) { }

	virtual void bind() const = 0;

	vk::Pipeline pipeline;

	const RenderTarget* renderTarget;

	lsd::String hash;
};

class Swapchain {
public:
	Swapchain() = default;
	Swapchain(CommandQueue& commandQueue);
	void createSwapchain();
	void createAttachments();

	bool update(bool windowChanged = false); // returns if the Swapchain has updated or not

	void aquire();
	void begin();
	void present();

	uint32 imageIndex = 0;
	uint32 currentFrame = 0;

	bool lostSurface = false;
	bool invalidSwapchain = false;
	bool invalidAttachments = false;

	uint32 presentFamilyIndex = std::numeric_limits<uint32>::max();
	vk::Queue presentQueue;

	vk::Surface surface;
	vk::Swapchain swapchain;
	vk::Swapchain oldSwapchain;

	lsd::Dynarray<Image, config::maxSwapchainImages> images;
	VkExtent2D extent;
	
	Image colorImage;
	GPUMemory colorMem;

	Image depthImage;
	GPUMemory depthMem;

	lsd::Array<vk::Semaphore, config::maxFramesInFlight> imageAquiredSemaphores;
	lsd::Array<vk::Semaphore, config::maxFramesInFlight> submitFinishedSemaphores;
	lsd::Array<vk::Fence, config::maxFramesInFlight> renderFinishedFences;

	CommandQueue* commandQueue;
	CommandQueue::CommandBuffer commandBuffer;
};

class RenderTarget {
public:
	static constexpr uint32 externalSubpass = VK_SUBPASS_EXTERNAL;

	struct Attachment {
		enum class Type {
			input,
			color,
			render,
			depth
		};

		struct MemoryModes {
			GPUMemory::LoadMode load = GPUMemory::LoadMode::dontCare;
			GPUMemory::StoreMode store = GPUMemory::StoreMode::dontCare;
			GPUMemory::LoadMode stencilLoad = GPUMemory::LoadMode::dontCare;
			GPUMemory::StoreMode stencilStore = GPUMemory::StoreMode::dontCare;
		};

		struct Dependencies {
			bool enabled = false;

			uint32 dependentPass;
			Pipeline::Stage stageDependencies = Pipeline::Stage::none;
			Pipeline::Stage stageWait = Pipeline::Stage::none;
			GPUMemory::Access memDependencies = GPUMemory::Access::none;
			GPUMemory::Access memWait = GPUMemory::Access::none;
		};

		struct Ranges {
			uint32 mipLayer = 0;
			uint32 mipCount = 1;
			uint32 arrayLayer = 0;
			uint32 arrayCount = 1;
		};

		lsd::Vector<const Image*> images;

		Type type;
		Image::Layout layout;
		Image::Layout initialLayout;
		Image::Layout finalLayout;
		Image::Aspect aspect;

		uint32 subpass = 0;

		MemoryModes memoryModes = { };
		Dependencies dependencies = { };
		Ranges ranges = { };
	};

	class Framebuffer {
	public:
		Framebuffer() = default;
		Framebuffer(uint32 index, const RenderTarget& renderTarget, const glm::u32vec2& size);

		lsd::Vector<Image::Resource> imageResources;
		vulkan::vk::Framebuffer framebuffer;

		glm::ivec2 size;
	};

	RenderTarget() = default;
	RenderTarget(const lsd::Vector<Attachment>& attachments, const glm::u32vec2& size = { std::numeric_limits<uint32>::max(), std::numeric_limits<uint32>::max() });
	~RenderTarget();

	RenderTarget& operator=(RenderTarget&&) noexcept = default;

	void createFramebuffers(const glm::u32vec2& size = { std::numeric_limits<uint32>::max(), std::numeric_limits<uint32>::max() });

	void begin() const;
	void end() const;

	lsd::Vector<Attachment> attachments;
	
	vk::RenderPass renderPass;
	lsd::Dynarray<Framebuffer, config::maxSwapchainImages> framebuffers;
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
		lsd::Vector<VkDescriptorImageInfo> infos;
		uint32 binding;
		Type type;
	};

	struct BufferWrite {
		lsd::Vector<VkDescriptorBufferInfo> infos;
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

	constexpr void addWrites(const lsd::Vector<ImageWrite>& newWrites) noexcept {
		dirty = true;

		imageWrites.reserve(imageWrites.size() + newWrites.size());
		imageWrites.insert(imageWrites.end(), newWrites.begin(), newWrites.end());
	}
	constexpr void addWrites(const lsd::Vector<BufferWrite>& newWrites) noexcept {
		dirty = true;

		bufferWrites.reserve(bufferWrites.size() + newWrites.size());
		bufferWrites.insert(bufferWrites.end(), newWrites.begin(), newWrites.end());
	}

	void update(uint32 index = std::numeric_limits<uint32>::max());

	void addDescriptorSets(uint32 count);

	void bind(uint32 index);

	lsd::Vector<vk::DescriptorSet> descriptorSets;

	lsd::Vector<ImageWrite> imageWrites;
	lsd::Vector<BufferWrite> bufferWrites;
	lsd::Vector<VkWriteDescriptorSet> writes;

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
	DescriptorPools(const lsd::Vector<Size>& sizes, Flags flags = Flags::freeDescriptorSet);

	void reset();
	vk::DescriptorSet allocate(const GraphicsProgram& program, uint32 layoutIndex, bool variableCount);

	lsd::Vector<vk::DescriptorPool> descriptorPools;

	uint32 allocationIndex = 0;

	VkDescriptorPoolCreateInfo createInfo;
	lsd::Vector<VkDescriptorPoolSize> sizes;
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
	Shader(Type type, lsd::Vector<char>&& source);
	Shader(Type type, const lsd::Vector<char>& source);

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
	lsd::Vector<char> shaderSrc;

	Type type;
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
			const lsd::Vector<VkSampler>& immutableSamplers = { };
		};
		
		struct PushConstant {
			// shader the push constant is in
			Shader::Type shaderType;
			// size of the push constant
			uint32 size;
		};

		constexpr void addBinding(const Binding& binding) {
			using namespace lsd::enum_operators;

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
			
			if ((binding.flags & Flags::variableCount) == Flags::variableCount) {
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
				lsd::toString(binding.type) + 
				lsd::toString(binding.shaderType) + 
				lsd::toString(binding.set) + 
				lsd::toString(binding.arraySize) + 
				lsd::toString(binding.flags) + 
				lsd::toString(binding.immutableSamplers.size())
			);
		}
		constexpr void addBindings(const lsd::Vector<Binding>& bindings) {
			for (const auto& binding : bindings) addBinding(binding);
		}

		constexpr void addPushConstant(const PushConstant& pushConstant) {
			m_pushConstants.pushBack({
				static_cast<VkShaderStageFlags>(pushConstant.shaderType),
				m_pushConstants.back().size + m_pushConstants.back().offset,
				pushConstant.size
			});

			m_pushConstantHash.append(lsd::toString(pushConstant.shaderType) + lsd::toString(pushConstant.size));
		}
		constexpr void addPushConstants(const lsd::Vector<PushConstant>& pushConstants) {
			for (const auto& pushConstant : pushConstants) addPushConstant(pushConstant);
		}

		constexpr void setVertexShader(const Shader& shader) {
			m_vertexShader = &shader;
		}
		constexpr void setFragmentShader(const Shader& shader) {
			m_fragmentShader = &shader;
		}

		lsd::String hash() const noexcept;

	private:
		lsd::Dynarray<lsd::Vector<VkDescriptorSetLayoutBinding>, config::maxShaderSets> m_bindings;
		lsd::Dynarray<lsd::Vector<VkDescriptorBindingFlags>, config::maxShaderSets> m_bindingFlags;
		lsd::Dynarray<VkDescriptorSetLayoutBindingFlagsCreateInfo, config::maxShaderSets> m_bindingFlagsCreateInfo;
		lsd::Vector<VkPushConstantRange> m_pushConstants;

		const Shader* m_vertexShader;
		const Shader* m_fragmentShader;

		lsd::String m_bindingHash;
		lsd::String m_pushConstantHash;

		friend class GraphicsProgram;
	};

	// Constructs a shader program in engine standard layout
	// You should prefer to write your shaders in the engine standard layout
	GraphicsProgram();
	// Constructs a shader program with a custom layout
	GraphicsProgram(const Builder& builder);

	lsd::Dynarray<vk::DescriptorSetLayout, config::maxShaderSets> descriptorSetLayouts;
	lsd::Dynarray<uint32, config::maxShaderSets> dynamicDescriptorCounts;
	vk::PipelineLayout pipelineLayout;

	const Shader* vertexShader;
	const Shader* fragmentShader;

	lsd::String hash;
};

class GraphicsPipeline : public Pipeline {
public:
	static constexpr Pipeline::BindPoint bindPoint = Pipeline::BindPoint::graphics;

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
		constexpr void addBlendAttachments(const lsd::Vector<VkPipelineColorBlendAttachmentState>& attachments) noexcept {
			m_blendAttachments.insert(m_blendAttachments.end(), attachments.begin(), attachments.end());
		}

		constexpr void setRenderTarget(const RenderTarget& renderTarget) {
			m_renderTarget = &renderTarget;
		}
		constexpr void setGraphicsProgram(const GraphicsProgram& graphicsProgram) {
			m_graphicsProgram = &graphicsProgram;
		}

		NODISCARD lsd::String hash() const noexcept;

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
		lsd::Vector<VkPipelineColorBlendAttachmentState> m_blendAttachments; // I have no idea what this does @todo

		const RenderTarget* m_renderTarget;
		const GraphicsProgram* m_graphicsProgram;

		friend class GraphicsPipeline;
	};

	// create a graphics pipeline with default engine properties
	GraphicsPipeline();
	// create a graphics pipeline with custom properties
	GraphicsPipeline(const Builder& builder);

	void bind() const;

	std::variant<bool, VkViewport> dynamicViewport;
	std::variant<bool, VkRect2D> dynamicScissor;

	const GraphicsProgram* program;
};

class ComputeProgram {
public:

};

class ComputePipeline : public Pipeline {
public:

};

class ImGuiRenderer : public lyra::ImGuiRenderer {
public:
	ImGuiRenderer();
	~ImGuiRenderer();

	// Call this after adding all fonts
	void uploadFonts();

private:
	void beginFrame();
	void endFrame();

	vulkan::DescriptorPools m_descriptorPools;
	vulkan::RenderTarget m_renderTarget;
};

class RenderSystem {
public:
	struct QueueFamilies {
		uint32 graphicsFamilyIndex = std::numeric_limits<uint32>::max();
		uint32 computeFamilyIndex = std::numeric_limits<uint32>::max();
		uint32 copyFamilyIndex = std::numeric_limits<uint32>::max();
		// uint32 videoFamilyIndex = VK_QUEUE_FAMILY_IGNORED; @todo
		
		lsd::Vector<VkQueueFamilyProperties> queueFamilyProperties;
	};

	RenderSystem(
		const lsd::Array<uint32, 3>& version,
		lsd::StringView defaultVertexShaderPath, 
		lsd::StringView defaultFragmentShaderPath
	);

	void initRenderComponents();

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
	VkResult allocateMemoryPages(const VkMemoryRequirements& memoryRequirements, const VmaAllocationCreateInfo& createInfo, lsd::Vector<VmaAllocation>& allocations, VmaAllocationInfo& allocationInfo) {
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
	void freeMemoryPages(const lsd::Vector<VmaAllocation>& allocations) {
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
	VkResult flushAllocations(const lsd::Vector<VmaAllocation>& allocations, const lsd::Vector<VkDeviceSize> offsets, const lsd::Vector<VkDeviceSize> sizes) {
		return vmaFlushAllocations(allocator, static_cast<uint32>(allocations.size()), allocations.data(), offsets.data(), sizes.data());
	}
	VkResult invalidateAllocation(const vma::Allocation& allocation, VkDeviceSize offset, VkDeviceSize size) {
		return vmaInvalidateAllocation(allocator, allocation.get(), offset, size);
	}
	VkResult invalidateAllocations(const lsd::Vector<VmaAllocation>& allocations, const lsd::Vector<VkDeviceSize> offsets, const lsd::Vector<VkDeviceSize> sizes) {
		return vmaInvalidateAllocations(allocator, static_cast<uint32>(allocations.size()), allocations.data(), offsets.data(), sizes.data());
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
	VkResult flushMappedMemoryRanges(const lsd::Vector<VkMappedMemoryRange>& memoryRanges) {
		return vkFlushMappedMemoryRanges(device, static_cast<uint32>(memoryRanges.size()), memoryRanges.data());
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
	void getImageSparseMemoryRequirements(const vk::Image& image, lsd::Vector<VkSparseImageMemoryRequirements>& sparseMemoryRequirements) {
		uint32 s;
		vkGetImageSparseMemoryRequirements(device, image, &s, nullptr);
		sparseMemoryRequirements.resize(s);
		vkGetImageSparseMemoryRequirements(device, image, &s, sparseMemoryRequirements.data());
	}
	void getImageSubresourceLayout(const vk::Image& image, const VkImageSubresource& subresource, VkSubresourceLayout& layout) {
		vkGetImageSubresourceLayout(device, image, &subresource, &layout);
	}
	VkResult getPipelineCacheData(const vk::PipelineCache& pipelineCache, size_type& pDataSize, void* pData) {
		return vkGetPipelineCacheData(device, pipelineCache, &pDataSize, pData);
	}
	VkResult getQueryPoolResults(const vk::QueryPool& queryPool, uint32 firstQuery, uint32 queryCount, size_type dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags) {
		return vkGetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
	}
	void getRenderAreaGranularity(const vk::RenderPass& renderPass, VkExtent2D& pGranularity) {
		vkGetRenderAreaGranularity(device, renderPass, &pGranularity);
	}
	VkResult invalidateMappedMemoryRange(const VkMappedMemoryRange& memoryRange) {
		return vkInvalidateMappedMemoryRanges(device, 1, &memoryRange);
	}
	VkResult invalidateMappedMemoryRanges(const lsd::Vector<VkMappedMemoryRange>& memoryRanges) {
		return vkInvalidateMappedMemoryRanges(device, static_cast<uint32>(memoryRanges.size()), memoryRanges.data());
	}
	VkResult mapMemory(const vma::Allocation& allocation, void** ppData) {
		return vmaMapMemory(allocator, allocation, ppData);
	}
	VkResult mergePipelineCache(const vk::PipelineCache& dstCache, const vk::PipelineCache srcCache) {
		return vkMergePipelineCaches(device, dstCache, 1, &srcCache.get());
	}
	VkResult mergePipelineCaches(const vk::PipelineCache& dstCache, const lsd::Vector<VkPipelineCache>& srcCaches) {
		return vkMergePipelineCaches(device, dstCache, static_cast<uint32>(srcCaches.size()), srcCaches.data());
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
	void updateDescriptorSet(const lsd::Vector<VkWriteDescriptorSet>& descriptorWrites) {
		vkUpdateDescriptorSets(device, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
	void updateDescriptorSet(const lsd::Vector<VkWriteDescriptorSet>& descriptorWrites, const lsd::Vector<VkCopyDescriptorSet>& descriptorCopies) {
		vkUpdateDescriptorSets(device, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), static_cast<uint32>(descriptorCopies.size()), descriptorCopies.data());
	}
	VkResult waitForFence(const vk::Fence& fence, VkBool32 waitAll, uint64 timeout) {
		return vkWaitForFences(device, 1, &fence.get(), waitAll, timeout);
	}
	VkResult waitForFences(const lsd::Vector<VkFence>& fences, VkBool32 waitAll, uint64 timeout) {
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

	vk::PipelineCache pipelineCache; // Implement the pipeline cache @todo

	lsd::UniquePointer<CommandQueue> commandQueue;
	lsd::UniquePointer<Swapchain> swapchain;
	lsd::UniquePointer<DescriptorPools> descriptorPools;

	lsd::Vector<RenderTarget*> renderTargets;
	lsd::UnorderedSparseMap<lsd::String, const GraphicsProgram*> graphicsPrograms;
	lsd::UnorderedSparseMap<lsd::String, GraphicsPipeline*> graphicsPipelines;

	RenderTarget* defaultRenderTarget;
	const GraphicsProgram* defaultGraphicsProgram;

	std::filesystem::path defaultVertexShaderPath;
	std::filesystem::path defaultFragmentShaderPath;

	const Shader* defaultVertexShader;
	const Shader* defaultFragmentShader;

	Entity* sceneRoot;

	lsd::Vector<Camera*> cameras;
	lsd::UnorderedSparseMap<GraphicsPipeline*, lsd::Vector<const Material*>> materials;
	lsd::UnorderedSparseMap<Material*, lsd::Vector<const MeshRenderer*>> meshRenderers;

	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

	float32 deltaTime;
};

} // namespace vulkan

} // namespace lyra
