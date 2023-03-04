/*************************
 * @file   cubemap.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief  a cubemap/skybox
 * 
 * @date   2022-19-8
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <lyra.h>

#include <core/array.h>
#include <vulkan/vulkan.h>
#include <rendering/vulkan/vulkan_raii.h>

#include <rendering/vulkan/descriptor.h>
#include <rendering/vulkan/vulkan_image.h>
#include <rendering/vulkan/GPU_memory.h>
#include <rendering/graphics_pipeline.h>

#include <nodes/mesh/mesh.h>
#include <nodes/mesh/mesh_renderer.h>

namespace lyra {

// cubemap base class
class CubemapBase : public vulkan::Image, public vulkan::GPUMemory, public GraphicsPipeline {
public:	
	CubemapBase(
		const Array<const char*, 6>& paths,
		const char* vertexShaderPath,
		const char* fragShaderPath,
		Camera* const camera,
		const VkFormat& format,
		const ColorBlending& colorBlending,
		const Tessellation& tessellation,
		const Multisampling& multisampling
	);

	/**
	 * @brief draw the cubemap
	 * 
	 */
	void draw() const;

	/**
	 * @brief get the information to bind to a descriptor
	 * 
	 * @param layout image layout
	 * 
	 * @return VkDescriptorImageInfo
	*/
	NODISCARD constexpr VkDescriptorImageInfo get_descriptor_cubemap_info(const VkImageLayout& layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) const noexcept {
		return {
			m_sampler,
			m_view,
			layout
		};
	}

	/**
	 * @brief get the sampler
	 * 
	 * @return consexprt const lyra::vulkan::vk::Sampler&
	*/
	NODISCARD constexpr const vulkan::vk::Sampler& sampler() const noexcept { return m_sampler; }

private:
	vulkan::vk::Sampler m_sampler;

	std::vector<vulkan::Descriptor> m_descriptorSets;
	Mesh m_cubeMesh;
	MeshRenderer m_cubeMeshRenderer;
};

// cubemap
class Cubemap : public CubemapBase {
public:
	Cubemap(
		const Array<const char*, 6>& paths,
		Script* script,
		Camera* const camera,
		const VkFormat& format = VK_FORMAT_R8G8B8A8_SRGB,
		const ColorBlending& colorBlending = ColorBlending::BLEND_ENABLE,
		const Tessellation& tessellation = Tessellation::TESSELLATION_ENABLE,
		const Multisampling& multisampling = Multisampling::MULTISAMPLING_ENABLE
	) : CubemapBase(paths, "data/shader/cubemapVert.spv", "data/shader/cubemapFrag.spv", camera, format, colorBlending, tessellation, multisampling) { }
};

// skybox
class Skybox : public CubemapBase {
public:
	Skybox(
		const Array<const char*, 6>& paths,
		Camera* const camera,
		const VkFormat& format = VK_FORMAT_R8G8B8A8_SRGB,
		const ColorBlending& colorBlending = ColorBlending::BLEND_ENABLE,
		const Tessellation& tessellation = Tessellation::TESSELLATION_ENABLE,
		const Multisampling& multisampling = Multisampling::MULTISAMPLING_ENABLE
	) : CubemapBase(paths, "data/shader/skyboxVert.spv", "data/shader/skyboxFrag.spv", camera, format, colorBlending, tessellation, multisampling) 
	{ camera->m_skybox = this; }
};

} // namespace lyra
