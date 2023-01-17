/**
 * @file frame.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 * 
 * @brief a virtual vulkan frame class
 * 
 * @date 2023-01-17
 * 
 * @copyright Copyright (c) 2023
 */

#pragma once

#include <lyra.h>

#include <vulkan/vulkan.h>

#include <rendering/vulkan/command_buffer.h>

namespace lyra {

namespace vulkan {

/**
 * @brief structure containing frame data and synchronisation objects
 */
class Frame {
public:
    /**
     * @brief construct the frame
     */
    Frame() : m_commandBuffer(CommandBuffer::Usage::USAGE_RENDERING_DEFAULT) {
        create_sync_objects();
    }
    /**
     * @brief destruct the frame
     */
    virtual ~Frame();
    /**
     * @brief recreate the frame
     *
     * @param index index of the frame in the renderer
     */
    void recreate();
    
    /**
     * @brief wait for Vulkan fences
     */
    void wait() const;
    /**
     * @brief reset Vulkan fences
     */
    void reset() const;

    /** 
     * @brief get the image available semaphores
     *
     * @return const VkSemaphor&
    */
    NODISCARD const VkSemaphore& imageAvailableSemaphores() const noexcept { return m_imageAvailableSemaphore; }
    /**
     * @brief get the render finished semaphores
     *
     * @return const VkSemaphore&
    */
    NODISCARD const VkSemaphore& renderFinishedSemaphores() const noexcept { return m_renderFinishedSemaphore; }
    /**
     * @brief get the in flight fences
     *
     * @return const VkFence&
    */
    NODISCARD const VkFence& inFlightFences() const noexcept { return m_inFlightFence; }
    /**
     * @brief get the in flight fences
     *
     * @return constexpr const CommandBuffer&
    */
    NODISCARD constexpr const CommandBuffer& commandBuffer() const noexcept { return m_commandBuffer; }

private:
    VkSemaphore m_imageAvailableSemaphore;
    VkSemaphore m_renderFinishedSemaphore;
    VkFence m_inFlightFence;

    CommandBuffer m_commandBuffer;

    /**
     * @brief create the syncronisation objects
     */
    void create_sync_objects();
};

} // namespace vulkan

} // namespace lyra