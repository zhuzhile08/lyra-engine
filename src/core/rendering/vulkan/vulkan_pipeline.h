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
#include <core/rendering/vulkan/descriptor.h>
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
	 * @brief get the descriptor set layout
	 *
	 * @return const lyra::VulkanDescriptorSetLayout*
	*/
	[[nodiscard]] const VulkanDescriptorSetLayout* const descriptorSetLayout() const noexcept { return &_descriptorSetLayout; }
	/**
	 * @brief get the descriptor pool
	 *
	 * @return const lyra::VulkanDescriptorPool*
	*/
	[[nodiscard]] const VulkanDescriptorPool* const descriptorPool() const noexcept { return &_descriptorPool; }
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
	VulkanDescriptorSetLayout _descriptorSetLayout;
	VulkanDescriptorPool _descriptorPool;

	VkPipelineBindPoint _bindPoint;

	std::vector<VulkanShader> _shaders;

	/**
	 * @brief create the pipeline layout
	 */
	void create_layout() {
		// create the pipeline layout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			nullptr,
			0,
			1,
			_descriptorSetLayout.get_ptr(),
			0,    /// @todo push constants
			nullptr
		};

		if (vkCreatePipelineLayout(Application::context()->device()->device(), &pipelineLayoutInfo, nullptr, &_layout) != VK_SUCCESS)
			Logger::log_exception("Failed to create Vulkan graphics pipeline layout!");
	}

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

	/**
	 * @brief create stuff related to descriptors
	 * 
	 * @param layoutBuilder builder for the descriptor set layout
	 * @param poolBuilder builder for the descriptor pool
	*/
	void create_descriptor_stuff(VulkanDescriptorSetLayout::Builder layoutBuilder, VulkanDescriptorPool::Builder poolBuilder) {
		_descriptorSetLayout.create(layoutBuilder);
		_descriptorPool.create(poolBuilder);
	}
};

} // namespace lyra
