/*************************
 * @file scene.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief a 3d scene
 * 
 * @date 2022-03-14
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/defines.h>
#include <core/logger.h>
#include <rendering/vulkan_wrappers.h>
#include <noud.h>

#include <vulkan.h>
#include <vector>

namespace lyra {

/**
 * @brief a Vulkan scene struct for easy loading and structuring 
 */
struct Scene : noud::Node {
    Scene();

    void destroy();

    void create();
};

} // namespace lyra
