/*************************
 * @file uniform_buffer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief uniform buffer objects
 * 
 * @date 2022-03-07
 * 
 * @copyright Copyright (c) 2022 Zhile Zhu
 *************************/

#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <rendering/vulkan/GPU_buffer.h>
#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/swapchain.h>
#include <core/logger.h>
#include <core/defines.h>

#include <vector>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <chrono>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief wrapper arond a uniform buffer object
 */
class Camera {
public:
    /**
     * @brief the uniform buffer object itself, containing all the position data needed
     */
    struct          UniformBufferObject {
        glm::mat4   model;
        glm::mat4   view;
        glm::mat4   proj;
    };

private:
    /**
     * @brief struct containing all the variabels
     */
    struct          Variables {
        std::vector<VulkanGPUBuffer>    buffers;
        UniformBufferObject             ubo;
		glm::vec3 						position;
		float 							aspect;
    };

public:
    Camera();

    /**
     * @brief destroy the camera
     */
    void            destroy() noexcept;

    /**
     * @brief create the camera
     * 
     * @param device device
     * @param ubo data of the ubo
     */
    void            create(VulkanDevice device, const UniformBufferObject && ubo, const VulkanSwapchain swapchain);

    /**
     * @brief rotate the camera around an certain axis
     * 
     * @param deg degrees of rotation in degrees
     * @param axis axis to rotate around
     */
    void            rotate(const float deg, const glm::vec3 axis) noexcept;
    /**
     * @brief set the rotation of the camera around an axis
     * 
     * @param deg degrees of rotation in degrees
     * @param axis axis to set the rotation of
     */
    void            set_rotation(const float deg, const glm::vec3 axis) noexcept;
	/**
	 * @brief move the camera by a certain amount
	 * 
	 * @param pos amount to move the camera
	 */
    void            move(const glm::vec3 pos) noexcept;
	/**
	 * @brief set the position of the camera
	 * 
	 * @param pos position to set to
	 */
	void 			set_position(const glm::vec3 pos) noexcept;
	/**
	 * @brief make the camera look at a position
	 * 
	 * @param target target to look at
	 * @param up normalized camera direction
	 */
	void 			look_at(const glm::vec3 target, const glm::vec3 up = {0.0f, 0.0f, 0.1f}) noexcept;
    /**
     * @brief set the perspective of the camera
     * 
     * @param swapchain swapchain
     * @param fov field of view
     * @param near near clipping plane
     * @param far far clipping plane
     */
    void            set_perspective(const float aspect, const float fov = FOV, const float near = 0.1f, const float far = 10.0f) noexcept;
    /**
     * @brief copy all the data of the ubo into the buffers after all transformations are applied
     * 
     * @param index index of the buffer
     */
    void            finish(uint32 index);

    /**
     * @brief get all the variables
     * 
     * @return Variables 
     */
    Variables       get() noexcept;

private:
    Variables       var;

    VulkanDevice*   device;
};

} // namespace lyra
