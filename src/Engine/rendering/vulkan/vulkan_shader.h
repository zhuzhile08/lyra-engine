/*************************
 * @file vulkan_shader.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief wrappers around the Vulkan shaders
 * 
 * @date 2022-02-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <core/defines.h>
#include <res/loaders/load_file.h>
#include <rendering/vulkan/devices.h>
#include <core/logger.h>

#include <vector>

#include <vulkan/vulkan.h>

namespace lyra {
/**
 * @brief wrapper around the Vulkan graphics pipeline
 */
class VulkanShader {
private:
	/**
	 * @brief struct containing all variables;
	 */
	struct Variables {
		VkShaderModule 					module;
		VkPipelineShaderStageCreateInfo	stage;
		std::string                     entry;
	};

public:
	VulkanShader();

	/**
	 * @brief destroy the Shader
	 */
	void destroy();

	/**
	 * @brief create a shader
	 * 
	 * @param device device
	 * @param path path of the shader
	 * @param entry name of the entrance point of the shader
	 * @param stage type of the shader
	 * @param shader the shader to create
	 * @return VkPipelineShaderStageCreateInfo
	 */
	void							create(VulkanDevice device, const std::string path, const str entry, VkShaderStageFlagBits stage);

	/**
	 * @brief get all variables
	 * 
	 * @return Variables
	 */
	Variables 						get() const;
    
private:
	Variables 						var;

	VulkanDevice*					device;
};

} // namespace lyra
