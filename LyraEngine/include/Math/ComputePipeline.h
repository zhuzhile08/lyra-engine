/*************************
 * @file ComputePipeline.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper around the Vulkan compute pipeline
 *
 * @date 2022-06-18
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Lyra/Lyra.h>

#include <Common/Utility.h>
#include <Common/Logger.h>

#include <Graphics/VulkanImpl/PipelineBase.h>

#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief Vulkan compute pipelines
*/
class ComputePipeline : public vulkan::Pipeline {
public:
	// compute pipeline builder
	class Builder : public vulkan::Pipeline::Builder {
	public: 
		/**
		 * @brief add the information for the single compute shader
		 * 
		 * @param shaderInfo shader information
		 */
		void add_shader_info(const ShaderInfo& shaderInfo) {
			if (m_shaderInfos.size() == 1) {
				m_shaderInfos.push_back(shaderInfo);
			} 
#ifndef NDEBUG
			else {
				log().warning("Compute Pipeline at address: ", get_address(this), " already has a shader assigned to it!");
			}
#endif
		}
		void add_shader_infos(const std::vector<ShaderInfo>& shaderInfos) = delete;
	private:
		/**
		 * @brief build the compute pipeline
		 * 
		 * @param computePipeline compute pieline to build
		 */
		void build_compute_pipeline(ComputePipeline* const computePipeline) const;

		friend class ComputePipeline;
	};

	ComputePipeline() = default;
	/**
	 * @brief construct a new compute pipeline
	 * 
	 * @param builder builder that contains the information to build the pipeline
	 */
	ComputePipeline(const Builder& builder);
	DEFINE_DEFAULT_MOVE(ComputePipeline)

	friend class Builder;
};

} // namespace lyra
