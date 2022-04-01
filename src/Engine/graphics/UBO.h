/*************************
 * @file UBO.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief Uniform Buffer Objects, the basis for cameras
 *
 * @date 2022-02-26
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <glm.hpp>

namespace lyra {

/**
* @brief uniform buffer object, containing camera position data
*/
struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

} // namespace lyra