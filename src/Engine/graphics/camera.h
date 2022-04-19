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

#include <graphics/GPU_buffer.h>
#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/swapchain.h>
#include <core/rendering/vulkan/descriptor.h>
#include <core/rendering/context.h>
#include <core/logger.h>
#include <core/defines.h>
#include <core/queue_types.h>
#include <math/math.h>

#include <vector>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief wrapper arond a uniform buffer object
 */
class Camera {
public:
	struct CameraData {
		CameraData();

		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	Camera();

	/**
	 * @brief destructor of the camera
	 */
	virtual ~Camera() noexcept;

	/**
	 * @brief destroy the camera
	 */
	void destroy() noexcept;

	Camera operator=(const Camera&) = delete;

	/**
	 * @brief create the camera
	 *
	 * @param context context
	 */
	void create(const Context* context);

	/**
	 * @brief rotate the camera around an certain axis
	 *
	 * @param deg degrees of rotation in degrees
	 * @param axis axis to rotate around
	 */
	void rotate(const float deg, const glm::vec3 axis) noexcept;
	/**
	 * @brief set the rotation of the camera around an axis
	 *
	 * @param deg degrees of rotation in degrees
	 * @param axis axis to set the rotation of
	 */
	void set_rotation(const float deg, const glm::vec3 axis) noexcept;
	/**
	 * @brief move the camera by a certain amount
	 *
	 * @param pos amount to move the camera
	 */
	void move(const glm::vec3 pos) noexcept;
	/**
	 * @brief set the position of the camera
	 *
	 * @param pos position to set to
	 */
	void set_position(const glm::vec3 pos) noexcept;
	/**
	 * @brief make the camera look at a position
	 *
	 * @param target target to look at
	 * @param up normalized camera direction
	 */
	void look_at(const glm::vec3 target, const glm::vec3 up = { 0.0f, 0.0f, 0.1f }) noexcept;
	/**
	 * @brief set the perspective of the camera
	 *
	 * @param swapchain swapchain
	 * @param fov field of view
	 * @param near near clipping plane
	 * @param far far clipping plane
	 */
	void set_perspective(const float aspect, const float fov = FOV, const float near = 0.1f, const float far = 10.0f) noexcept;

	/**
	 * @brief temporary draw function
	*/
	void draw();

	/**
	 * @brief get the GPU memory buffers
	 * 
	 * @return const std::vector<VulkanGPUBuffer>
	*/
	[[nodiscard]] const std::vector<VulkanGPUBuffer> buffers() const noexcept { return _buffers; }
	/**
	 * @brief get the camera data of the camera
	 * 
	 * @return const CameraData
	*/
	[[nodiscard]] const CameraData data() const noexcept { return _data; }
	/**
	 * @brief get the queue with the update commands
	 * 
	 * @return const CallQueue
	*/
	[[nodiscard]] const CallQueue& updateQueue() const noexcept { return _updateQueue; }
	/**
	 * @brief get the position of the camera
	 * 
	 * @return const glm::vec3
	*/
	[[nodiscard]] const glm::vec3 position() const noexcept { return _position; }
	/**
	 * @brief get the aspect ratio of the camera
	 * 
	 * @return const float
	*/
	[[nodiscard]] const float aspect() const noexcept { return _aspect; }

private:
	std::vector<VulkanGPUBuffer> _buffers;
	CameraData _data;
	CallQueue _updateQueue;

	glm::vec3 _position = glm::vec3(2.0f);
	float _aspect;

	const Context* context;
};

} // namespace lyra
