/*************************
 * @file instance.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief a wrapper around a Vulkan instance
 * 
 * @date 2022-02-04
 * 
 * @copyright Copyright (c) 2022 Zhile Zhu
 *************************/

#pragma once

#include <core/defines.h>
#include <core/logger.h>

#include <SDL.h>
#include <SDL_vulkan.h>
#include <rendering/window.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief a wrapper around the Vulkan instance
 */
class VulkanInstance {
private:
    /**
     * @brief struct containing the variables of a class
     */
    struct          Variables {
        VkInstance      instance;
        VkSurfaceKHR    surface;
    };

public:
    VulkanInstance();
    
    /**
     * @brief destroy a Instance
     */
    void            destroy() noexcept;

    /**
     * @brief create the instance and surface
     * 
     * @param window window
     */
    void            create(Window window);

    /**
     * @brief get all variabels
     * 
     * @return Variables
     */
    Variables       get() const noexcept;

private:
    Variables       var;

    Window*         window;

    /**
     * @brief check if a vector of user requested Vulkan validation layers is actually available
     * 
     * @param layers the available validation layers
     * @param requestedLayers the requested validation layers
     */
    void            check_requested_validation_layers(const std::vector <VkLayerProperties> layers, const std::vector <str> requestedLayers) const;
    /**
     * @brief create a Vulkan instance
     * 
     * @param window window
     */
    void            create_instance();

    /**
     * @brief create a Vulkan window surface
     * 
     * @param window window
     */
    void            create_window_surface();
};

} // namespace lyra
