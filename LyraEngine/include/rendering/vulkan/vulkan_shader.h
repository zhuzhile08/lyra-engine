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

#include <vulkan/vulkan.h>

namespace lyra {

namespace vulkan {
/**
 * @brief wrapper around the Vulkan graphics pipeline
 */
class Shader {
public:
	// types of shaders
	enum Type : int {
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
	Shader(const char* path, const char* entry, const Type& type);
	/**
	* @brief destructor of the shader
	**/
#ifndef ndebug
	virtual ~Shader();
#else
	virtual ~Shader() {
		vkDestroyShaderModule(Application::renderSystem()->device()->device(), m_module, nullptr);
	}
#endif

	Shader operator=(const Shader&) const noexcept = delete;

	/**
	 * @brief get the shader loading information
	 *
	 * @return constexpr const VkPipelineShaderStageCreateInfo
	*/
	NODISCARD constexpr const VkPipelineShaderStageCreateInfo get_stage_create_info() const noexcept {
		return {
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			static_cast<VkShaderStageFlagBits>(m_type),
			m_module,
			m_entry,
			nullptr
		};
	}

	/**
	 * @brief get the shader module
	 * 
	 * @return constexpr const VkShaderModule&
	*/
	NODISCARD constexpr const VkShaderModule& module() const noexcept { return m_module; }
	/**
	 * @brief get the entry point of the shader
	 * 
	 * @return constexpr const string
	*/
	NODISCARD constexpr const char* const entry() const noexcept { return m_entry; }

private:
	VkShaderModule m_module = VK_NULL_HANDLE;
	Type m_type;
	const char* m_entry;
};

} // namespace vulkan

} // namespace lyra
