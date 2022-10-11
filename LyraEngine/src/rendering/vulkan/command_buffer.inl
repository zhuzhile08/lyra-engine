#include <rendering/vulkan/command_buffer.h>

#include <vulkan/vulkan.h>

#include <core/util.h>

namespace lyra {

namespace vulkan {

const uint32 CommandBufferManager::get_unused() const {
    for (auto& it : m_commandBuffers) 
        if (it.second == CommandBufferUsage::COMMAND_BUFFER_UNUSED) 
            return it.first; 
#ifndef NDEBUG
    Logger::log_exception("Failed to get an unused command buffer from the command buffer manager at: ", get_address(this), "!");
#endif
    return 0;
};

void CommandBuffer::bindDescriptorSets(
	VkPipelineBindPoint pipelineBindPoint,
	VkPipelineLayout layout,
	uint32 firstSet,
	const VkDescriptorSet pDescriptorSets[],
	const uint32 pDynamicOffsets[]) {
	vkCmdBindDescriptorSets(*m_commandBuffer, pipelineBindPoint, layout, firstSet, arr_size(pDescriptorSets), pDescriptorSets, arr_size(pDynamicOffsets), pDynamicOffsets);
}
void CommandBuffer::blitImages(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, const VkImageBlit pRegions[], VkFilter filter) {
	vkCmdBlitImage(*m_commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, arr_size(pRegions), pRegions, filter);
}
void CommandBuffer::clearAttachments(uint32 attachmentCount, const VkClearAttachment* pAttachments, const VkClearRect* pRects) {
	vkCmdClearAttachments(*m_commandBuffer, attachmentCount, pAttachments, arr_size(pRects), pRects);
}
void CommandBuffer::clearAttachments(uint32 attachmentCount, const VkClearAttachment& pAttachment, const VkClearRect* pRects) {
	vkCmdClearAttachments(*m_commandBuffer, attachmentCount, &pAttachment, arr_size(pRects), pRects);
}
void CommandBuffer::clearColorImage(VkImage image, VkImageLayout imageLayout, const VkClearColorValue& pColor, const VkImageSubresourceRange pRanges[]) {
	vkCmdClearColorImage(*m_commandBuffer, image, imageLayout, &pColor, arr_size(pRanges), pRanges);
}
void CommandBuffer::clearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue& pDepthStencil, const VkImageSubresourceRange pRanges[]) {
	vkCmdClearDepthStencilImage(*m_commandBuffer, image, imageLayout, &pDepthStencil, arr_size(pRanges), pRanges);
}
void CommandBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, const VkBufferCopy pRegions[]) {
	vkCmdCopyBuffer(*m_commandBuffer, srcBuffer, dstBuffer, arr_size(pRegions), pRegions);
}
void CommandBuffer::copyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, const VkBufferImageCopy pRegions[]) {
	vkCmdCopyBufferToImage(*m_commandBuffer, srcBuffer, dstImage, dstImageLayout, arr_size(pRegions), pRegions);
}
void CommandBuffer::copyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, const VkImageCopy pRegions[]) {
	vkCmdCopyImage(*m_commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, arr_size(pRegions), pRegions);
}
void CommandBuffer::copyImageToBuffer(VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, const VkBufferImageCopy pRegions[]) {
	vkCmdCopyImageToBuffer(*m_commandBuffer, srcImage, srcImageLayout, dstBuffer, arr_size(pRegions), pRegions);
}
void CommandBuffer::executeCommands(const VkCommandBuffer pCommandBuffers[]) {
	vkCmdExecuteCommands(*m_commandBuffer, arr_size(pCommandBuffers), pCommandBuffers);
}
void CommandBuffer::pipelineBarrier(
	VkPipelineStageFlags srcStageFlags,
	VkPipelineStageFlags dstStageFlags,
	const VkMemoryBarrier memory[],
	const VkBufferMemoryBarrier buffer[],
	const VkImageMemoryBarrier image[],
	VkDependencyFlags dependency
) {
	vkCmdPipelineBarrier(
		*m_commandBuffer,
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
void CommandBuffer::resolveImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, const VkImageResolve pRegions[]) {
	vkCmdResolveImage(*m_commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, arr_size(pRegions), pRegions);
}
void CommandBuffer::setScissor(uint32 firstScissor, const VkRect2D pScissors[]) {
	vkCmdSetScissor(*m_commandBuffer, firstScissor, arr_size(pScissors), pScissors);
}
void CommandBuffer::setViewport(uint32 firstViewport, const VkViewport pViewports[]) {
	vkCmdSetViewport(*m_commandBuffer, firstViewport, arr_size(pViewports), pViewports);
}
void CommandBuffer::waitEvents(
	const VkEvent pEvents[],
	VkPipelineStageFlags srcStageMask,
	VkPipelineStageFlags dstStageMask,
	const VkMemoryBarrier pMemoryBarriers[],
	const VkBufferMemoryBarrier pBufferMemoryBarriers[],
	const VkImageMemoryBarrier pImageMemoryBarriers[]
) {
	vkCmdWaitEvents(
		*m_commandBuffer, 
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

} // namespace vulkan

} // namespace lyra
