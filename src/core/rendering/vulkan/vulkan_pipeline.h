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
	// types of shaders
	enum class ShaderType {
		// vertex shader
		TYPE_VERTEX = 0x00000001,
		// tessellation control shader
		TYPE_TESSELLATION_CONTROL = 0x00000002,
		// tessellation evaluation shader
		TYPE_TESSELLATION_EVALUATION = 0x00000004,
		// geometry shader
		TYPE_GEOMETRY = 0x00000008,
		// fragment shader
		TYPE_FRAGMENT = 0x00000010,
		// shader containing every graphics stage
		TYPE_GRAPHICS = 0x0000001F,
		// compute shader
		TYPE_COMPUTE = 0x00000020,
		// all shader types
		TYPE_ALL = 0x7FFFFFFF,
		// ray generation shader
		TYPE_RAY_GENERATION = 0x00000100,
		// ray hit detection shader
		TYPE_RAY_ANY_HIT = 0x00000200,
		// ray closest hit shader
		TYPE_RAY_CLOSEST_HIT = 0x00000400,
		// ray miss shader
		TYPE_RAY_MISS = 0x00000800,
		// ray intersection shader
		TYPE_RAY_INTERSECTION = 0x00001000,
		// read the Vulkan API docs, I have no idea what this does
		TYPE_CALLABLE = 0x00002000,
		// read the Vulkan API docs, I have no idea what this does
		TYPE_TASK = 0x00000040,
		// read the Vulkan API docs, I have no idea what this does
		TYPE_MESH = 0x00000080,
	}; // I totally didn't steal these names from the API, why would I?

	/**
	 * @brief struct holding creation information for a shader
	 */
	struct ShaderCreationInfo {
		// shader type
		const ShaderType type;
		// path of shader
		const std::string path;
		// shader entry point
		const std::string entry = "main";
	};

public:
	VulkanPipeline();

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
	 * @brief create a new graphics pipeline
	 */
	virtual void create();

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
	 * @brief create the graphics pipeline
	 */
	virtual void create_pipeline();
	/**
	 * @brief create the pipeline layout
	 */
	virtual void create_layout();

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
			_shaders.at(index).create(Application::context()->device(), shaderCreationInfos.at(index).path, shaderCreationInfos.at(index).entry, static_cast<VkShaderStageFlagBits>(shaderCreationInfos.at(index).type));
			Logger::log_info("Succesfully created Vulkan shader at: ", get_address(&_shaders.at(index)), " with flag: ", shaderCreationInfos.at(index).type, "!");
		}
	}
};

} // namespace lyra
