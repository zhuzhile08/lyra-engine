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

#include <lyra.h>

#include <vector>

#include <vulkan/vulkan.h>

#include <core/smart_pointer.h>

#include <rendering/vulkan/vulkan_shader.h>
#include <rendering/vulkan/descriptor.h>

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
		 */
		void set_descriptor_pool_flags(const VkDescriptorPoolCreateFlags& poolFlags) noexcept {
			m_poolFlags = poolFlags;
		}

	private:
		std::vector<ShaderInfo> m_shaderInfos;
		std::vector<BindingInfo> m_bindingInfos;
		std::vector<PushConstantInfo> m_pushConstantInfos;
		VkDescriptorPoolCreateFlags m_poolFlags = 0;

		/**
		 * @brief build the creation info for the pipeline layout
		 * 
		 * @param descriptorSystem descriptor system which contains the set layout for this pipeline
		 * 
		 * @return const VkPipelineLayoutCreateInfo 
		 */
		NODISCARD const VkPipelineLayoutCreateInfo build_layout_create_info(const DescriptorSystem& descriptorSystem) const noexcept;
		/**
		 * @brief build the builder for the descriptor set layout
		 * 
		 * @return const lyra::vulkan::DescriptorSystem::LayoutBuilder 
		 */
		NODISCARD const DescriptorSystem::LayoutBuilder build_set_layout_builder() const;
		/**
		 * @brief build the builder for the descriptor pool
		 * 
		 * @return const lyra::vulka DescriptorSystem::PoolBuilder 
		 */
		NODISCARD const DescriptorSystem::PoolBuilder build_pool_builder() const;

		friend class Pipeline;
	};

	/**
	 * @brief destructor of the pipeline
	 */
	virtual ~Pipeline() noexcept;

	Pipeline operator=(const Pipeline&) const noexcept = delete;

	/**
	 * @brief get the descriptor management system
	 *
	 * @return constexpr lyra::vulkan::DescriptorSystem&
	*/
	NODISCARD constexpr DescriptorSystem& descriptorSystem()noexcept { return m_descriptorSystem; }
	/**
	 * @brief get the pipeline
	 *
	 * @return constexpr VkPipeline
	*/
	NODISCARD constexpr VkPipeline pipeline() const noexcept { return m_pipeline; }
	/**
	 * @brief get the pipeline layout
	 *
	 * @return constexpr VkPipelineLayout
	*/
	NODISCARD constexpr VkPipelineLayout layout() const noexcept { return m_layout; }
	/**
	 * @brief get the shaders
	 *
	 * @return const std::vector<Shader>
	*/
	NODISCARD const std::vector<Shader> shaders() const noexcept { return m_shaders; }
	/**
	 * @brief get the shaders
	 *
	 * @return constexpr VkPipelineBindPoint
	*/
	NODISCARD constexpr VkPipelineBindPoint bindPoint() const noexcept { return m_bindPoint; }

protected:
	VkPipeline m_pipeline;
	VkPipelineLayout m_layout;
	DescriptorSystem m_descriptorSystem;

	VkPipelineBindPoint m_bindPoint;

	std::vector<Shader> m_shaders;

	/**
	 * @brief create all the shaders
	 *
	 * @param builder containing the shader information
	 */
	void create_shaders(const Builder* const builder);
	/**
	 * @brief create stuff related to descriptors
	 * 
	 * @param builder builder which contains the descriptor informations
	*/
	void create_descriptor_stuff(const Builder* const builder);
	/**
	 * @brief create the pipeline layout
	 * 
	 * @param builder builder containing the info necceceary to create the pipeline layout
	 */
	void create_layout(const Builder* const builder);
};

} // namespace vulkan

} // namespace lyra
