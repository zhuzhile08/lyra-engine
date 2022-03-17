/*************************
 * @file swapchain.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief wrapper around a Vulkan swapchain and related stuff
 * 
 * @date 2022-02-05
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/defines.h>
#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/instance.h>
#include <rendering/vulkan/vulkan_image.h>
#include <core/logger.h>

#include <vector>

#include <SDL.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief Wrapper around a Vulkan Swapchain
 */
class VulkanSwapchain {
private:
    /**
     * @brief wrapper around swapchain images
     */
    struct          VulkanSwapchainImages {
    public:
        VulkanSwapchainImages();

        /**
         * @brief destroy the swapchain images
         */
        void destroy();
        
        /**
         * @brief create the image views
         * 
         * @param device device
         * @param format swapchain format
         * @param swapchain swapchain
         */
        void                        create(VulkanDevice device, const VulkanSwapchain swapchain);

        std::vector <VulkanImage>   images;

    private:
        VulkanDevice*               device;
    };
    
    /**
     * @brief wrapper around depth buffers
     */
    struct          VulkanDepthBuffer {
    public:
        VulkanDepthBuffer();

        /**
         * @brief destroy the depth buffer
         */
        void            destroy();

        /**
         * @brief create the image, view and allocate the memory
         * 
         * @param device device
         * @param swapchain swapchain
         */
        void            create(VulkanDevice device, const VulkanSwapchain swapchain);

        VulkanImage     image;
        VmaAllocation   memory;

    private:
        VulkanDevice*   device;
    };

    /**
     * @brief struct containing all the variables
     */
    struct          Variables {
        VkSwapchainKHR          swapchain;
        VkFormat                format;
        VkExtent2D              extent;
        VulkanSwapchainImages   images;
        VulkanDepthBuffer       depthBuffer;

        VulkanSwapchain*        oldSwapchain = nullptr;
    };

public:
    VulkanSwapchain();

    /**
     * @brief destroy the VulkanSwapchain
     */
    void            destroy();

    /**
     * @brief create the swapchain
     * 
     * @param device device
     * @param instance instance
     * @param window window
     */
    void            create(VulkanDevice device, VulkanInstance instance, Window window);

    /**
     * @brief create the swapchain
     * 
     * @param oldSwapchain old swapchain
     */
    void            create(VulkanSwapchain oldSwapchain);

    /**
     * @brief get all variables
     * 
     * @return Variables
     */
    Variables       get() const;
    
private:
    Variables       var;

    VulkanDevice*   device;
    VulkanInstance* instance;
    Window*         window;

    /**
     * @brief create the swapchain
     */
    void            create_swapchain();
    /**
     * @brief create a extent of the swapchain 
     *
     * @param surfaceCapabilities capabilities of the swapchain
     */
    void            create_swapchain_extent(VkSurfaceCapabilitiesKHR *surfaceCapabilities);
};

} // namespace lyra
