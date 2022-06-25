/*************************
 * @file graphics_pipeline.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper around the Vulkan compute pipeline
 * @brief loads shaders and processes the vertices passing through
 *
 * @date 2022-06-18
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/rendering/vulkan/vulkan_pipeline.h>
#include <core/rendering/vulkan/vulkan_shader.h>
#include <core/logger.h>
#include <lyra.h>

#include <vector>

#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief Vulkan compute pipelines
*/
class ComputePipeline : public VulkanPipeline {
public:
	// creation information
	struct CreateInfo {
		// shader creation information
		const ShaderCreationInfo shaderCreationInfo;
		// descriptor set layout and pool creation information
		const Builder builder;
	};

	ComputePipeline() { }

	/**
	 * @brief create the compute pipeline
	 * 
	 * @param info creation information
	*/
	void create(const CreateInfo info);

private:
	/**
	 * @brief create the compute pipeline
	 *
	 * @param info creation information
	 */
	void create_pipeline(const CreateInfo info);
};

} // namespace lyra
