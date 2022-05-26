/*************************
 * @file vulkan_pipeline.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief empty wrapper around the Vulkan pipeline
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/defines.h>
#include <res/loaders/load_file.h>
#include <core/rendering/vulkan/vulkan_shader.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/logger.h>
#include <lyra.h>

#include <vector>

#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief wrapper around the Vulkan graphics pipeline
 */
class VulkanPipeline {
protected:
	/**
	 * @brief struct holding creation information for a shader
	 */
	struct ShaderCreationInfo {
		// shader type
		const VulkanShader::Type type;
		// path of shader
		const std::string path;
		// shader entry point
		const std::string entry = "main";
	};

public:
	VulkanPipeline() { };

	/**
	 * @brief destructor of the pipeline
	 */
	virtual ~VulkanPipeline() noexcept {
		vkDestroyPipeline(Application::context()->device()->device(), _pipeline, nullptr);
		vkDestroyPipelineLayout(Application::context()->device()->device(), _layout, nullptr);
		_shaders.clear();

		Logger::log_info("Succesfully destroyed Vulkan pipeline!");
	}

	/**
	 * @brief destroy the pipeline
	 */
	virtual void destroy() noexcept { this->~VulkanPipeline(); };

	VulkanPipeline operator=(const VulkanPipeline&) const noexcept = delete;

	/**
	 * @brief bind the pipeline
	 * 
	 * @param buffer command buffer to bind with
	*/
	void bind(const VulkanCommandBuffer* const buffer) { vkCmdBindPipeline(buffer->get(), _bindPoint, _pipeline); }

	/**
	 * @brief get the pipeline
	 *
	 * @return const VkPipeline&
	*/
	[[nodiscard]] const VkPipeline& pipeline() const noexcept { return _pipeline; }
	/**
	 * @brief get the pipeline layout
	 *
	 * @return const VkPipelineLayout&
	*/
	[[nodiscard]] const VkPipelineLayout& layout() const noexcept { return _layout; }
	/**
	 * @brief get the shaders
	 *
	 * @return const std::vector<VulkanShader>&
	*/
	[[nodiscard]] const std::vector<VulkanShader>& shaders() const noexcept { return _shaders; }
	/**
	 * @brief get the shaders
	 *
	 * @return const VkPipelineBindPoint&
	*/
	[[nodiscard]] const VkPipelineBindPoint& bindPoint() const noexcept { return _bindPoint; }

protected:
	VkPipeline _pipeline = VK_NULL_HANDLE;
	VkPipelineLayout _layout = VK_NULL_HANDLE;

	VkPipelineBindPoint _bindPoint;

	std::vector<VulkanShader> _shaders;

	/**
	 * @brief create all the shaders
	 *
	 * @param shaderCreationInfos creation information of the shaders
	 */
	void create_shaders(std::vector<ShaderCreationInfo> shaderCreationInfos) {
		if (shaderCreationInfos.size() != _shaders.size()) {
			Logger::log_warning("Number of shader creation infos doesn't match up with the numbers of shaders in the pipeline at: ", get_address(this), "!");
		}

		for (int index = 0; index < shaderCreationInfos.size(); index++) {
			_shaders.at(index).create(Application::context()->device(), shaderCreationInfos.at(index).path, shaderCreationInfos.at(index).entry, shaderCreationInfos.at(index).type);
			Logger::log_info("Succesfully created Vulkan shader at: ", get_address(&_shaders.at(index)), " with flag: ", shaderCreationInfos.at(index).type, "!");
		}
	}
};

} // namespace lyra
