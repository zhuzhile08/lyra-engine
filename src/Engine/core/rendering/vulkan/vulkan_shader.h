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
#include <core/rendering/vulkan/devices.h>
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
	* @brief destructor of the shader
	**/
	virtual ~VulkanShader() noexcept;

	/**
	 * @brief destroy the shader
	 */
	void destroy() noexcept;

	VulkanShader operator=(const VulkanShader&) const noexcept = delete;

	/**
	 * @brief create a shader
	 *
	 * @param device device
	 * @param path path of the shader
	 * @param entry name of the entrance point of the shader
	 * @param stage type of the shader
	 */
	void create(const VulkanDevice* const device, const string path, const string entry, VkShaderStageFlagBits stage);

	/**
	 * @brief get the shader loading information
	 *
	 * @return const VkPipelineShaderStageCreateInfo
	*/
	[[nodiscard]] const VkPipelineShaderStageCreateInfo get_stage_create_info() const noexcept;

	/**
	 * @brief get the shader module
	 * 
	 * @return const VkShaderModule
	*/
	[[nodiscard]] const VkShaderModule module() const noexcept { return _module; }
	/**
	 * @brief get the entry point of the shader
	 * 
	 * @return const string
	*/
	[[nodiscard]] const string entry() const noexcept { return _entry; }

private:
	VkShaderModule _module = VK_NULL_HANDLE;
	VkShaderStageFlagBits _stageFlags;
	string _entry;

	const VulkanDevice* device;
};

} // namespace lyra
