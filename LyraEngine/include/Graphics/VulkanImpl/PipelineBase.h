/*************************
 * @file PipelineBase.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief empty wrapper around the Vulkan pipeline
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Lyra/Lyra.h>

#include <vector>
#include <Common/Array.h>
#include <Common/SmartPointer.h>

#include <Common/Settings.h>

#include <vulkan/vulkan.h>

#include <Graphics/VulkanImpl/Shader.h>
#include <Graphics/VulkanImpl/DescriptorSystem.h>

namespace lyra {

namespace vulkan {

/**
 * @brief wrapper around the Vulkan graphics pipeline
 */
class Pipeline {
public:
	/**
	 * @brief A helper class to build the pipeline and its elements
	 */
	class Builder {
	public:
		/**
		 * @brief struct holding creation information for a shader
		 */
		struct ShaderInfo {
			// shader type
			const uint32& type;
			// path of shader
			const char* path;
			// shader entry point
			const char* entry = "main";
		};

		/**
		 * @brief simplified descriptor and shader information
		 */
		struct BindingInfo {
			// shader type
			const uint32& shaderType;
			// the descriptor set layout this binding belongs to
			const uint32& descriptorSetLayoutIndex;
			// type of descriptor
			const uint32& descriptorType;
			// multiplier for the number of descriptors to allocate
			const uint32& descriptorAllocCountMultiplier;
			// number of descriptors in that slot (array)
			const uint32& arraySize = 1;
		};

		/**
		 * @brief structure containing information for push constants
		 */
		struct PushConstantInfo {
			// type of shader
			const Shader::Type shaderType;
			// size of push constant
			const uint32& size;
			// offset of the push constant
			const uint32& offset = 0;
		};

		/**
		 * @brief add a single shader information to the internal vector
		 * 
		 * @param shaderInfo shader information
		 */
		void add_shader_info(const ShaderInfo& shaderInfo) {
			m_shaderInfos.push_back(shaderInfo);
		}
		/**
		 * @brief add multiple shader informations to the internal vector
		 * 
		 * @param shaderInfos vector of shader informations
		 */
		void add_shader_infos(const std::vector<ShaderInfo>& shaderInfos) {
			for (const auto& shaderInfo : shaderInfos) add_shader_info(shaderInfo);
		}

		/**
		 * @brief add a single descriptor binding information to the internal vector
		 * 
		 * @param bindingInfo descriptor binding information
		 */
		void add_binding_info(const BindingInfo& bindingInfo) {
			m_bindingInfos.push_back(bindingInfo);
		}
		/**
		 * @brief add multiple descriptor binding informations to the internal vector
		 * 
		 * @param bindingInfos vector of descriptor binding informations
		 */
		void add_binding_infos(const std::vector<BindingInfo>& bindingInfos) {
			for (const auto& bindingInfo : bindingInfos) add_binding_info(bindingInfo);
		}

		/**
		 * @brief add a single push constant information to the internal vector
		 * 
		 * @param pushConstantInfo push constant information
		 */
		void add_push_constant_info(const PushConstantInfo& pushConstantInfo) {
			m_pushConstantInfos.push_back(pushConstantInfo);
		}
		/**
		 * @brief add multiple push constant informations to the internal vector
		 * 
		 * @param pushConstantInfos vector of push constant informations
		 */
		void add_push_constant_infos(const std::vector<PushConstantInfo>& pushConstantInfos) {
			for (const auto& pushConstantInfo : pushConstantInfos) add_push_constant_info(pushConstantInfo);
		}

		/**
		 * @brief set the descriptor pool flags
		 *
		 * @param poolFlags pool creation flags
		 * @param index index of the pool
		 */
		void set_descriptor_pool_flags(const VkDescriptorPoolCreateFlags& poolFlags, const uint32& index) noexcept {
			m_poolFlags[index] = poolFlags;
		}

		/**
		 * @brief build the base for the pipeline
		 * 
		 * @param pipeline pipeline which contains the layout to be built
		 */
		void build_pipeline_base(Pipeline* pipeline) const noexcept;

	protected:
		std::vector<ShaderInfo> m_shaderInfos;
		std::vector<BindingInfo> m_bindingInfos;
		std::vector<PushConstantInfo> m_pushConstantInfos;
		std::vector<VkDescriptorPoolCreateFlags> m_poolFlags;

		friend class GraphicsPipeline;
		friend class ComputePipeline;
	};

	/**
	 * @brief get the descriptor management systems
	 *
	 * @return std::vector<lyra::vulkan::DescriptorSystem>&
	*/
	NODISCARD constexpr std::vector<DescriptorSystem>& descriptorSystems() noexcept { return m_descriptorSystems; }
	/**
	 * @brief get a specific descriptor management system
	 * 
	 * @param index index of the descriptor system
	 *
	 * @return std::vector<lyra::vulkan::DescriptorSystem>&
	*/
	NODISCARD constexpr DescriptorSystem& descriptorSystem(const uint32& index) noexcept { return m_descriptorSystems[index]; }
	/**
	 * @brief get the pipeline
	 *
	 * @return lyra::vulkan::vk::Pipeline&
	*/
	NODISCARD constexpr const vk::Pipeline& pipeline() const noexcept { return m_pipeline; }
	/**
	 * @brief get the pipeline layout
	 *
	 * @return lyra::vulkan::vk::PipelineLayout&
	*/
	NODISCARD constexpr const vk::PipelineLayout& layout() const noexcept { return m_layout; }
	/**
	 * @brief get the shaders
	 *
	 * @return const std::vector<Shader>&
	*/
	NODISCARD constexpr const std::vector<Shader>& shaders() const noexcept { return m_shaders; }
	/**
	 * @brief get the shaders
	 *
	 * @return VkPipelineBindPoint
	*/
	NODISCARD constexpr VkPipelineBindPoint bindPoint() const noexcept { return m_bindPoint; }

protected:
	vk::Pipeline m_pipeline;
	vk::PipelineLayout m_layout;
	std::vector<DescriptorSystem> m_descriptorSystems;

	VkPipelineBindPoint m_bindPoint;

	std::vector<Shader> m_shaders;

	friend class Builder;
};

} // namespace vulkan

} // namespace lyra
