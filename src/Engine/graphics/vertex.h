/*************************
 * @file vertex.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a file with and mesh structs
 *
 * @date 2022-02-03
 *
 * @copyright Copyright (c) 2022 Zhile Zhu
 *************************/

#pragma once

#define GLM_FORCE_RADIANS

#include <vector>
#include <array>
#include <glm.hpp>
#include <vulkan/vulkan.h>

namespace lyra {

struct              Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 uv;

	Vertex();

	/**
	 * @brief construct a new Vertex object
	 *
	 * @param pos the new position
	 * @param normal vertex normals
	 * @param color the new color
	 */
	Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 uv, glm::vec3 color = { 0, 0, 0 });

	/**
	 * @brief returns a static vertex binding
	 *
	 * @return VkVertexInputBindingDescription
	 */
	static const VkVertexInputBindingDescription get_binding_description() noexcept;

	/**
	 * @brief returns a static vertex input attribute
	 *
	 * @return std::array<VkVertexInputAttributeDescription, 4>
	 */
	static const std::array<VkVertexInputAttributeDescription, 4> get_attribute_descriptions() noexcept;
};

} // namespace lyra
