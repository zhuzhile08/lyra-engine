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

/**
 * @brief command pool
 */
class VulkanCommandPool {
public:
	VulkanCommandPool() { }
	/**
	 * @brief create a Vulkan command pool to allocate the command buffers
	 *
	 * @param device device
	 */
	VulkanCommandPool(const VulkanDevice* const device);
	/**
	* @brief destructor of the command pool
	**/
	virtual ~VulkanCommandPool() noexcept;
	/**
	 * @brief destroy the command pool
	 */
	void destroy() noexcept {
		this->~VulkanCommandPool();
	}

	VulkanCommandPool operator=(const VulkanCommandPool&) const noexcept = delete;

	/**
	 * @brief reset the command buffer
	 */
	void reset();

	/**
	 * @brief get the command pool
	 *
	 * @return const VkCommandPool&
	 */
	[[nodiscard]] const VkCommandPool& commandPool() const noexcept { return _commandPool; }

private:
	VkCommandPool _commandPool = VK_NULL_HANDLE;

	const VulkanDevice* device;
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
		 * @param device device
		 * @param commandPool command pool
		 * @param level level of the command buffer
		 */
		VulkanCommandBuffer(const VulkanDevice* const device, const VulkanCommandPool* const commandPool, const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

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
		const VulkanDevice* device;
		const VulkanCommandPool* commandPool;
	};

	friend class RenderSystem;

public:
	CommandBufferManager() { }
	/**
	 * @brief create the command buffer manager
	 *
	 * @param device device
	 * @param commandPool command pool
	 * @param level level of the command buffers in the manager
	 */
	CommandBufferManager(const VulkanDevice* const device, const VulkanCommandPool* const commandPool, const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

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
	 * @brief begin recording a commandBuffer
	 *
	 * @param cmdBuffer the command buffer to perform the operation on
	 * @param usage what the recording will be used for
	 */
	void begin(const CommandBuffer cmdBuffer, const VkCommandBufferUsageFlags usage = 0);
	/**
	 * @brief end recording a commandBuffer
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
			_commandBufferData.at(cmdBuffer).commandBuffer,
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
	 */
	[[nodiscard]] const CommandBuffer& get_unused() { 
		for (auto& it : _commandBuffers) 
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
	 */
	[[nodiscard]] const VulkanCommandBuffer* const commandBuffer(CommandBuffer index) const noexcept { return &_commandBufferData.at(index); }

private:
	std::vector<VulkanCommandBuffer> _commandBufferData;
	std::unordered_map<CommandBuffer, CommandBufferUsage> _commandBuffers;
};

} // namespace lyra
