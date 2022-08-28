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

#include <core/decl.h>
#include <core/util.h>
#include <core/settings.h>

#include <map>
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
	/**
	 * @brief destroy the command pool
	 */
	void destroy() noexcept {
		this->~CommandPool();
	}

	CommandPool operator=(const CommandPool&) const noexcept = delete;

	/**
	 * @brief reset the command buffer
	 */
	void reset();

	/**
	 * @brief get the command pool
	 *
	 * @return const VkCommandPool&
	 */
	NODISCARD const VkCommandPool& commandPool() const noexcept { return m_commandPool; }

private:
	VkCommandPool m_commandPool = VK_NULL_HANDLE;
};

/**
 * @brief manager for command buffers
 */
class CommandBufferManager {
private:
	// enum to see if command buffer is used or not
	enum class CommandBufferUsage : unsigned int {
		COMMAND_BUFFER_USED,
		COMMAND_BUFFER_UNUSED
	};

	/**
	 * @brief command buffer
	 */
	struct VulkanCommandBuffer {
	public:
		VulkanCommandBuffer() { }
		/**
		 * @brief create the Vulkan command buffers
		 *
		 * @param commandPool command pool
		 * @param level level of the command buffer
		 */
		VulkanCommandBuffer(const CommandPool* const commandPool, const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		/**
		* @brief destructor of the command buffer
		**/
		virtual ~VulkanCommandBuffer() noexcept;
		/**
		 * @brief destroy the command buffer
		 */
		void destroy() noexcept {
			this->~VulkanCommandBuffer();
		}

		VulkanCommandBuffer operator=(const VulkanCommandBuffer&) const noexcept = delete;

		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
	
	private:
		const CommandPool* commandPool;
	};

	friend class RenderSystem;

public:
	/**
	 * @brief create the command buffer manager
	 *
	 * @param device device
	 * @param commandPool command pool
	 * @param level level of the command buffers in the manager
	 */
	CommandBufferManager(const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	/**
	 * @brief destructor of the command buffer manager
	 **/
	virtual ~CommandBufferManager() noexcept;
	/**
	 * @brief destroy the command buffer manager
	 */
	void destroy() noexcept {
		this->~CommandBufferManager();
	}

	CommandBufferManager operator=(const CommandBufferManager&) const noexcept = delete;

	/**
	 * @brief begin recording a VulkanCommandBuffer
	 *
	 * @param cmdBuffer the command buffer to perform the operation on
	 * @param usage what the recording will be used for
	 */
	void begin(const CommandBuffer cmdBuffer, const VkCommandBufferUsageFlags usage = 0);
	/**
	 * @brief end recording a VulkanCommandBuffer
	 * 
	 * @param cmdBuffer the command buffer to perform the operation on
	 */
	void end(const CommandBuffer cmdBuffer) const;
	/**
	 * reset the command buffer after everything has been recorded and make it available for usage again
	 *
	 * @param cmdBuffer the command buffer to perform the operation on
	 * @param flags additional flags
	 */
	void reset(const CommandBuffer cmdBuffer, const VkCommandBufferResetFlags flags = 0);

	/**
	 * @brief submit a Vulkan queue after command queue recording. DO NOT confuse with the submit function in the renderer class. This is ONLY for small, local submits for one time commands
	 *
	 * @param cmdBuffer the command buffer to perform the operation on
	 * @param queue the queue to submit
	 */
	void submit_queue(const CommandBuffer cmdBuffer, const VkQueue queue) const;

	/**
	 * @brief wait for the queue to finish
	 *
	 * @param queue the queue to wait for
	*/
	void wait_queue(const VkQueue queue) const;

	/**
	 * @brief setup a pipeline barrier
	 *
	 * @param cmdBuffer the command buffer to perform the operation on
	 * @param srcStageFlags the pipeline stage to wait for
	 * @param dstStageFlags the pipeline stage to skip to
	 * @param memory if there is, then the memory to transition
	 * @param buffer if there is, then the buffer to transition
	 * @param image if there is, then the image to transition
	 * @param dependency I have no idea, just leave it zero
	*/
	void pipeline_barrier(
		const CommandBuffer cmdBuffer,
		const VkPipelineStageFlags srcStageFlags,
		const VkPipelineStageFlags dstStageFlags,
		const VkMemoryBarrier* const memory = nullptr,
		const VkBufferMemoryBarrier* const buffer = nullptr,
		const VkImageMemoryBarrier* const image = nullptr,
		const VkDependencyFlags dependency = 0
	) const {
		vkCmdPipelineBarrier(
			m_commandBufferData.at(cmdBuffer).commandBuffer,
			srcStageFlags,
			dstStageFlags,
			dependency,
			(memory == nullptr) ? 0 : 1,
			memory,
			(buffer == nullptr) ? 0 : 1,
			buffer,
			(image == nullptr) ? 0 : 1,
			image
		);
	}

	/**
	 * @brief return the index of an unused pipeline
	 * 
	 * @return const lyra::CommandBuffer
	 */
	NODISCARD const CommandBuffer get_unused() const { 
		for (auto& it : m_commandBuffers) 
			if (it.second == CommandBufferUsage::COMMAND_BUFFER_UNUSED) 
				return it.first; 
#ifdef _DEBUG
		Logger::log_exception("Failed to get an unused command buffer from the command buffer manager at: ", get_address(this), "!");
#endif
		return 0;
	};

	/**
	 * @brief get a command buffer at a specific index
	 *
	 * @param index index of the command buffer
	 * 
	 * @return const lyra::vulkan::VulkanCommandBuffer* const
	 */
	NODISCARD const VulkanCommandBuffer* const commandBuffer(CommandBuffer index) const noexcept { return &m_commandBufferData.at(index); }

private:
	CommandPool m_commandPool;
	std::vector<VulkanCommandBuffer> m_commandBufferData;
	std::unordered_map<CommandBuffer, CommandBufferUsage> m_commandBuffers;
};

} // namespace vulkan

} // namespace lyra
