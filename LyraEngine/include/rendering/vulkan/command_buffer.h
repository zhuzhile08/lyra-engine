/*************************
 * @file command_buffer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a wrapper around the Vulkan command buffers
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <lyra.h>

#include <unordered_map>
#include <vulkan/vulkan.h>

namespace lyra {

namespace vulkan {

/**
 * @brief command pool
 */
class CommandPool {
public:
	CommandPool();

	/**
	* @brief destructor of the command pool
	**/
	virtual ~CommandPool() noexcept;

	CommandPool operator=(const CommandPool&) const noexcept = delete;

	/**
	 * @brief reset the command buffer
	 */
	void reset();

	/**
	 * @brief get the command pool
	 *
	 * @return constexpr const VkCommandPool&
	 */
	NODISCARD constexpr const VkCommandPool& commandPool() const noexcept { return m_commandPool; }

private:
	VkCommandPool m_commandPool;
};

class CommandBuffer {
public:
	// an enum to store the usage of the command Buffer
	enum class Usage : uint32 {
		USAGE_RENDERING_DEFAULT = 0x00000000,
		USAGE_ONE_TIME_SUBMIT = 0x00000001,
		USAGE_RENDER_PASS_CONTINUE = 0x00000002,
		USAGE_SIMULTANIOUS = 0x00000004
	};

	/**
	 * @brief construct the command buffer wrapper
	 *
	 * @param usage usage of the command buffer
	 */
	CommandBuffer(const Usage& usage = Usage::USAGE_RENDERING_DEFAULT, const VkCommandBufferLevel& level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	/**
	 * @brief destructor of the command buffer
	 */
	virtual ~CommandBuffer();
	
	CommandBuffer operator=(const CommandBuffer& commandBuffer) const noexcept = delete;

	/**
	 * @brief wrappers around the core Vulkan API command functions
	 * @brief these are basically copied directly from the Vulkan API with minor modifications to reduce bloat and suit a more modern C++ style
	 * @brief please refer to the official Vulkan documentation (preferably at https://devdocs.io/vulkan/ for the documentation of these functions
	 */

	void begin() const;
	void end() const {
		vassert(vkEndCommandBuffer(m_commandBuffer), "stop recording command buffer");
	}
	void beginQuery(const VkQueryPool& queryPool, const uint32& query, const VkQueryControlFlags& flags) {
		vkCmdBeginQuery(m_commandBuffer, queryPool, query, flags);
	}
	void beginRenderPass(const VkRenderPassBeginInfo& pRenderPassBegin, const VkSubpassContents& contents) const {
		vkCmdBeginRenderPass(m_commandBuffer, &pRenderPassBegin, contents);
	}
	void bindDescriptorSet(
		const VkPipelineBindPoint& pipelineBindPoint,
		const VkPipelineLayout& layout,
		const uint32& firstSet,
		const VkDescriptorSet& pDescriptorSet,
		const uint32& pDynamicOffset = UINT32_MAX
	) {
		vkCmdBindDescriptorSets(m_commandBuffer, pipelineBindPoint, layout, firstSet, 1, &pDescriptorSet, (pDynamicOffset == UINT32_MAX) ? 0 : 1, (pDynamicOffset == UINT32_MAX) ? nullptr : &pDynamicOffset);
	}
	void bindDescriptorSets(
		const VkPipelineBindPoint& pipelineBindPoint,
		const VkPipelineLayout& layout,
		const uint32& firstSet,
		const VkDescriptorSet pDescriptorSets[],
		const uint32 pDynamicOffsets[]
	) {
		vkCmdBindDescriptorSets(m_commandBuffer, pipelineBindPoint, layout, firstSet, arr_size(pDescriptorSets), pDescriptorSets, arr_size(pDynamicOffsets), pDynamicOffsets);
	}
	void bindIndexBuffer(const VkBuffer& buffer, const VkDeviceSize& offset, const VkIndexType indexType) {
		vkCmdBindIndexBuffer(m_commandBuffer, buffer, offset, indexType);
	}
	void bindPipeline(const VkPipelineBindPoint pipelineBindPoint, const VkPipeline pipeline) {
		vkCmdBindPipeline(m_commandBuffer, pipelineBindPoint, pipeline);
	}
	void bindVertexBuffer(const uint32& firstBinding, const uint32& bindingCount, const VkBuffer& pBuffer, const VkDeviceSize& pOffset = 0) {
		vkCmdBindVertexBuffers(m_commandBuffer, firstBinding, bindingCount, &pBuffer, &pOffset);
	}
	void bindVertexBuffers(const uint32& firstBinding, const uint32& bindingCount, const VkBuffer pBuffers[], const VkDeviceSize pOffsets[] = 0) {
		vkCmdBindVertexBuffers(m_commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
	}
	void blitImage(const VkImage& srcImage, const VkImageLayout& srcImageLayout, const VkImage& dstImage, const VkImageLayout& dstImageLayout, const VkImageBlit& pRegion, const VkFilter& filter) {
		vkCmdBlitImage(m_commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, 1, &pRegion, filter);
	}
	void blitImages(const VkImage& srcImage, const VkImageLayout& srcImageLayout, const VkImage& dstImage, const VkImageLayout& dstImageLayout, const VkImageBlit pRegions[], const VkFilter& filter) {
		vkCmdBlitImage(m_commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, arr_size(pRegions), pRegions, filter);
	}
	void clearAttachments(const uint32& attachmentCount, const VkClearAttachment& pAttachment, const VkClearRect& pRect) {
		vkCmdClearAttachments(m_commandBuffer, attachmentCount, &pAttachment, 1, &pRect);
	}
	void clearAttachments(const uint32& attachmentCount, const VkClearAttachment* pAttachments, const VkClearRect* pRects) {
		vkCmdClearAttachments(m_commandBuffer, attachmentCount, pAttachments, arr_size(pRects), pRects);
	}
	void clearAttachments(const uint32& attachmentCount, const VkClearAttachment& pAttachment, const VkClearRect* pRects) {
		vkCmdClearAttachments(m_commandBuffer, attachmentCount, &pAttachment, arr_size(pRects), pRects);
	}
	void clearColorImage(const VkImage&image, const VkImageLayout& imageLayout, const VkClearColorValue& pColor, const VkImageSubresourceRange& pRange) {
		vkCmdClearColorImage(m_commandBuffer, image, imageLayout, &pColor, 1, &pRange);
	}
	void clearColorImage (const VkImage& image, const VkImageLayout& imageLayout, const VkClearColorValue& pColor, const VkImageSubresourceRange pRanges[]) {
		vkCmdClearColorImage(m_commandBuffer, image, imageLayout, &pColor, arr_size(pRanges), pRanges);
	}
	void clearDepthStencilImage (const VkImage& image, const VkImageLayout& imageLayout, const VkClearDepthStencilValue& pDepthStencil, const VkImageSubresourceRange& pRange) {
		vkCmdClearDepthStencilImage(m_commandBuffer, image, imageLayout, &pDepthStencil, 1, &pRange);
	}
	void clearDepthStencilImage (const VkImage&image, const VkImageLayout& imageLayout, const VkClearDepthStencilValue& pDepthStencil, const VkImageSubresourceRange pRanges[]) {
		vkCmdClearDepthStencilImage(m_commandBuffer, image, imageLayout, &pDepthStencil, arr_size(pRanges), pRanges);
	}
	void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const VkBufferCopy& pRegion) {
		vkCmdCopyBuffer(m_commandBuffer, srcBuffer, dstBuffer, 1, &pRegion);
	}
	void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const VkBufferCopy pRegions[]) {
		vkCmdCopyBuffer(m_commandBuffer, srcBuffer, dstBuffer, arr_size(pRegions), pRegions);
	}
	void copyBufferToImage(const VkBuffer& srcBuffer, const VkImage& dstImage, const VkImageLayout& dstImageLayout, const VkBufferImageCopy& pRegion) {
		vkCmdCopyBufferToImage(m_commandBuffer, srcBuffer, dstImage, dstImageLayout, 1, &pRegion);
	}
	void copyBufferToImage(const VkBuffer& srcBuffer, const VkImage& dstImage, const VkImageLayout& dstImageLayout, const VkBufferImageCopy pRegions[]) {
		vkCmdCopyBufferToImage(m_commandBuffer, srcBuffer, dstImage, dstImageLayout, arr_size(pRegions), pRegions);
	}
	void copyImage (const VkImage& srcImage, const VkImageLayout& srcImageLayout, const VkImage& dstImage, const VkImageLayout& dstImageLayout, const VkImageCopy& pRegion) {
		vkCmdCopyImage(m_commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, 1, &pRegion);
	}
	void copyImage (const VkImage& srcImage, const VkImageLayout& srcImageLayout, const VkImage& dstImage, const VkImageLayout& dstImageLayout, const VkImageCopy pRegions[]) {
		vkCmdCopyImage(m_commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, arr_size(pRegions), pRegions);
	}
	void copyImageToBuffer (const VkImage& srcImage, const VkImageLayout& srcImageLayout, const VkBuffer& dstBuffer, const VkBufferImageCopy& pRegion) {
		vkCmdCopyImageToBuffer(m_commandBuffer, srcImage, srcImageLayout, dstBuffer, 1, &pRegion);
	}
	void copyImageToBuffer (const VkImage& srcImage, const VkImageLayout& srcImageLayout, const VkBuffer& dstBuffer, const VkBufferImageCopy pRegions[]) {
		vkCmdCopyImageToBuffer(m_commandBuffer, srcImage, srcImageLayout, dstBuffer, arr_size(pRegions), pRegions);
	}
	void copyQueryPoolResults(
		const VkQueryPool queryPool,
		const uint32& firstQuery,
		const uint32& queryCount,
		const VkBuffer& dstBuffer,
		const VkDeviceSize& dstOffset,
		const VkDeviceSize& stride,
		const VkQueryResultFlags& flags) {
		vkCmdCopyQueryPoolResults(m_commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
	}
	void dispatch(const uint32& groupCountX, const uint32& groupCountY, const uint32& groupCountZ) {
		vkCmdDispatch(m_commandBuffer, groupCountX, groupCountY, groupCountZ);
	}
	void dispatchIndirect(const VkBuffer& buffer, const VkDeviceSize& offset) {
		vkCmdDispatchIndirect(m_commandBuffer, buffer, offset);
	}
	void draw(const uint32& vertexCount, const uint32& instanceCount, const uint32& firstVertex, const uint32& firstInstance) {
		vkCmdDraw(m_commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
	}
	void drawIndexed(const uint32& indexCount, const uint32& instanceCount, const uint32& firstIndex, int32 vertexOffset, const uint32& firstInstance) {
		vkCmdDrawIndexed(m_commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}
	void drawIndexedIndirect(const VkBuffer& buffer, const VkDeviceSize& offset, const uint32& drawCount, const uint32& stride) {
		vkCmdDrawIndexedIndirect(m_commandBuffer, buffer, offset, drawCount, stride);
	}
	void drawIndirect(const VkBuffer& buffer, const VkDeviceSize& offset, const uint32& drawCount, const uint32& stride) {
		vkCmdDrawIndirect(m_commandBuffer, buffer, offset, drawCount, stride);
	}
	void endQuery(const VkQueryPool& queryPool, const uint32& query) {
		vkCmdEndQuery(m_commandBuffer, queryPool, query);
	}
	void endRenderPass() {
		vkCmdEndRenderPass(m_commandBuffer);
	}
	void executeCommands(const VkCommandBuffer& pCommandBuffer) {
		vkCmdExecuteCommands(m_commandBuffer, 1, &pCommandBuffer);
	}
	void executeCommands(const VkCommandBuffer pCommandBuffers[]) {
		vkCmdExecuteCommands(m_commandBuffer, arr_size(pCommandBuffers), pCommandBuffers);
	}
	void fillBuffer(const VkBuffer& dstBuffer, const VkDeviceSize& dstOffset, const VkDeviceSize& size, const uint32& data) {
		vkCmdFillBuffer(m_commandBuffer, dstBuffer, dstOffset, size, data);
	}
	void nextSubpass(const VkSubpassContents& contents) {
		vkCmdNextSubpass(m_commandBuffer, contents);
	}
	void pipelineBarrier(
		const VkPipelineStageFlags& srcStageFlags,
		const VkPipelineStageFlags& dstStageFlags,
		const VkMemoryBarrier& memory = VkMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM },
		const VkBufferMemoryBarrier& buffer = VkBufferMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM },
		const VkImageMemoryBarrier& image = VkImageMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM },
		const VkDependencyFlags& dependency = 0
	) {
		vkCmdPipelineBarrier(
			m_commandBuffer,
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
		const VkPipelineStageFlags& srcStageFlags,
		const VkPipelineStageFlags& dstStageFlags,
		const VkMemoryBarrier memory[] = { },
		const VkBufferMemoryBarrier buffer[] = { },
		const VkImageMemoryBarrier image[] = { },
		const VkDependencyFlags& dependency = 0
	) {
		vkCmdPipelineBarrier(
			m_commandBuffer,
			srcStageFlags,
			dstStageFlags,
			dependency,
			arr_size(memory),
			memory,
			arr_size(buffer),
			buffer,
			arr_size(image),
			image
		);
	}
	void pushConstants(const VkPipelineLayout& layout, const VkShaderStageFlags& stageFlags, const uint32& offset, const uint32& size, const void* pValues) {
		vkCmdPushConstants(m_commandBuffer, layout, stageFlags, offset, size, pValues);
	}
	void resetEvent(const VkEvent event, const VkPipelineStageFlags& stageMask) {
		vkCmdResetEvent(m_commandBuffer, event, stageMask);
	}
	void resetQueryPool(const VkQueryPool queryPool, const uint32& firstQuery, const uint32& queryCount) {
		vkCmdResetQueryPool(m_commandBuffer, queryPool, firstQuery, queryCount);
	}
	void resolveImage (const VkImage& srcImage, const VkImageLayout& srcImageLayout, const VkImage& dstImage, const VkImageLayout& dstImageLayout, const VkImageResolve& pRegion) {
		vkCmdResolveImage(m_commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, 1, &pRegion);
	}
	void resolveImage (const VkImage& srcImage, const VkImageLayout& srcImageLayout, const VkImage& dstImage, const VkImageLayout& dstImageLayout, const VkImageResolve pRegions[]) {
		vkCmdResolveImage(m_commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, arr_size(pRegions), pRegions);
	}
	void setBlendConstants(const float blendConstants[4]) {
		vkCmdSetBlendConstants(m_commandBuffer, blendConstants);
	}
	void setDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) {
		vkCmdSetDepthBias(m_commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
	}
	void setDepthBounds(float minDepthBounds, float maxDepthBounds) {
		vkCmdSetDepthBounds(m_commandBuffer, minDepthBounds, maxDepthBounds);
	}
	void setEvent(const VkEvent event, const VkPipelineStageFlags& stageMask) {
		vkCmdSetEvent(m_commandBuffer, event, stageMask);
	}
	void setLineWidth(float lineWidth) {
		vkCmdSetLineWidth(m_commandBuffer, lineWidth);
	}
	void setScissor(const uint32& firstScissor, const VkRect2D& pScissor) {
		vkCmdSetScissor(m_commandBuffer, firstScissor, 1, &pScissor);
	}
	void setScissor(const uint32& firstScissor, const VkRect2D pScissors[]) {
		vkCmdSetScissor(m_commandBuffer, firstScissor, arr_size(pScissors), pScissors);
	}
	void setStencilCompareMask(const VkStencilFaceFlags& faceMask, const uint32& compareMask) {
		vkCmdSetStencilCompareMask(m_commandBuffer, faceMask, compareMask);
	}
	void setStencilReference(const VkStencilFaceFlags& faceMask, const uint32& reference) {
		vkCmdSetStencilReference(m_commandBuffer, faceMask, reference);
	}
	void setStencilWriteMask(const VkStencilFaceFlags& faceMask, const uint32& writeMask) {
		vkCmdSetStencilWriteMask(m_commandBuffer, faceMask, writeMask);
	}
	void setViewport(const uint32& firstViewport, const VkViewport& pViewport) {
		vkCmdSetViewport(m_commandBuffer, firstViewport, 1, &pViewport);
	}
	void setViewport(const uint32& firstViewport, const VkViewport pViewports[]) {
		vkCmdSetViewport(m_commandBuffer, firstViewport, arr_size(pViewports), pViewports);
	}
	void updateBuffer(const VkBuffer& dstBuffer, const VkDeviceSize& dstOffset, const VkDeviceSize& dataSize, const void* pData) {
		vkCmdUpdateBuffer(m_commandBuffer, dstBuffer, dstOffset, dataSize, pData);
	}
	void waitEvents(
		const VkEvent& pEvent,
		const VkPipelineStageFlags& srcStageMask,
		const VkPipelineStageFlags& dstStageMask,
		const VkMemoryBarrier& pMemoryBarrier,
		const VkBufferMemoryBarrier& pBufferMemoryBarrier,
		const VkImageMemoryBarrier& pImageMemoryBarrier
	) {
		vkCmdWaitEvents(
			m_commandBuffer,
			1,
			&pEvent,
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
		const VkEvent pEvents[],
		const VkPipelineStageFlags& srcStageMask,
		const VkPipelineStageFlags& dstStageMask,
		const VkMemoryBarrier pMemoryBarriers[] = { },
		const VkBufferMemoryBarrier pBufferMemoryBarriers[] = { },
		const VkImageMemoryBarrier pImageMemoryBarriers[] = { }
	) {
		vkCmdWaitEvents(
			m_commandBuffer, 
			arr_size(pEvents), 
			pEvents, 
			srcStageMask, 
			dstStageMask, 
			arr_size(pMemoryBarriers), 
			pMemoryBarriers, 
			arr_size(pBufferMemoryBarriers), 
			pBufferMemoryBarriers, 
			arr_size(pImageMemoryBarriers), 
			pImageMemoryBarriers
		);
	}
	void writeTimestamp(const VkPipelineStageFlagBits pipelineStage, const VkQueryPool queryPool, const uint32& query) {
		vkCmdWriteTimestamp(m_commandBuffer, pipelineStage, queryPool, query);
	}
	void reset(const VkCommandBufferResetFlags& flags = 0) const;
	void submitQueue(const VkQueue& queue) const;

	/**
	 * @brief get the command buffer
	 * 
	 * @return const VkCommandBuffer&
	 */
	NODISCARD const VkCommandBuffer& commandBuffer() const noexcept { return m_commandBuffer; }

private:
	Usage m_usage;
	VkCommandBuffer m_commandBuffer;

	const CommandPool& m_commandPool;
};

} // namespace vulkan

} // namespace lyra
