/*************************
 * @file vulkan_shader.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrappers around the Vulkan shaders
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

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
public:
	VulkanShader();

	/**
	 * @brief destroy the Shader
	 */
	void destroy() noexcept;

	/**
	 * @brief create a shader
	 *
	 * @param device device
	 * @param path path of the shader
	 * @param entry name of the entrance point of the shader
	 * @param stage type of the shader
	 */
	void create(VulkanDevice device, const std::string path, const str entry, VkShaderStageFlagBits stage);

	/**
	 * @brief get the shader module
	 * 
	 * @return const VkShaderModule
	*/
	const VkShaderModule module() const noexcept { return _module; }
	/**
	 * @brief get the shader loading information
	 * 
	 * @return const VkPipelineShaderStageCreateInfo
	*/
	const VkPipelineShaderStageCreateInfo stage() const noexcept { return _stage; }
	/**
	 * @brief get the entry point of the shader
	 * 
	 * @return const std::string
	*/
	const std::string entry() const noexcept { return _entry; }

private:
	VkShaderModule _module;
	VkPipelineShaderStageCreateInfo	_stage;
	std::string _entry;

	VulkanDevice* device;
};

} // namespace lyra
