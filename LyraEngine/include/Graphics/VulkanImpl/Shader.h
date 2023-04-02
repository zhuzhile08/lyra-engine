/*************************
 * @file Shader.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrappers around the Vulkan shaders
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Lyra/Lyra.h>

#include <vulkan/vulkan.h>
#include <Common/RAIIContainers.h>

namespace lyra {

namespace vulkan {
/**
 * @brief wrapper around the Vulkan graphics pipeline
 */
class Shader {
public:
	// types of shaders
	enum Type { // sadly, I cannot use enum classes for this one
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
	 * @brief create a shader
	 *
	 * @param path path of the shader
	 * @param entry name of the entrance point of the shader
	 * @param type type of the shader
	 */
	Shader(std::string_view path, std::string_view entry, const Type& type);

	/**
	 * @brief get the shader loading information
	 *
	 * @return constexpr VkPipelineShaderStageCreateInfo
	*/
	NODISCARD constexpr VkPipelineShaderStageCreateInfo get_stage_create_info() const noexcept {
		return {
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			static_cast<VkShaderStageFlagBits>(m_type),
			m_module,
			m_entry.c_str(),
			nullptr
		};
	}

	/**
	 * @brief get the shader module
	 * 
	 * @return constexpr const lyra::vulkan::vk::ShaderModule&
	*/
	NODISCARD constexpr const vk::ShaderModule& module() const noexcept { return m_module; }
	/**
	 * @brief get the entry point of the shader
	 * 
	 * @return std::string_view
	*/
	NODISCARD constexpr std::string_view entry() const noexcept { return m_entry; }

private:
	vk::ShaderModule m_module;
	Type m_type;
	std::string m_entry;
};

} // namespace vulkan

} // namespace lyra
