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
 * @brief wrapper around a Vulkan shader
 */
class Shader {
public:
	// types of shaders
	enum Type : uint32 { 
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
		// all shader types
		TYPE_ALL = 0x0000FFFF
	}; // Refer to the API for the documentation of these enums

	// function/flags of shader
	enum Flags {
		// ambient lighting
		FLAG_SHADING_AMBIENT = 0x00010000,
		// diffuse lighting
		FLAG_SHADING_DIFFUSE = 0x00020000,
		// specular lighting
		FLAG_SHADING_SPECULAR = 0x00040000,
		// phong shading
		FLAG_SHADING_PHONG = FLAG_SHADING_AMBIENT | FLAG_SHADING_DIFFUSE | FLAG_SHADING_SPECULAR,
		// cel shading (characters)
		FLAG_SHADING_CEL = 0x00080000,
		// high detail cel shading (enviroment)
		FLAG_SHADING_CEL_HIGH_DETAIL = 0x00100000,

		// outlined shader
		FLAG_OUTLINE = 0x01000000,
		// fast outlined shader
		FLAG_OUTLINE_FAST = 0x02000000,

		// shader for armaturized meshes
		FLAG_ARMATURE = 0x10000000,

		// universally usable shader
		FLAG_UNIVERSAL = 0x7FFF0000
	};

	Shader() = default;
	/**
	 * @brief create a shader
	 *
	 * @param pathAndFlags path and flags of the shader, combined for technical reasons
	 * 
	 * @param entry name of the entrance point of the shader
	 */
	Shader(std::pair<std::string_view, uint32> pathAndFlags, std::string_view entry = "main");

	/**
	 * @brief get the shader loading information
	 *
	 * @return VkPipelineShaderStageCreateInfo
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
	 * @return const lyra::vulkan::vk::ShaderModule&
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
	Flags m_flags;
	std::string m_entry;
};

} // namespace vulkan

} // namespace lyra
