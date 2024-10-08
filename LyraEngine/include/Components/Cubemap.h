/*************************
 * @file Cubemap.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief a cubemap/skybox
 * 
 * @date 2022-19-8
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <Common/Common.h>

#include <LSD/Array.h>
#include <vulkan/vulkan.h>
#include <Common/RAIIContainers.h>

#include <Graphics/VulkanImpl/DescriptorSystem.h>
#include <Graphics/VulkanImpl/Image.h>
#include <Graphics/VulkanImpl/GPUMemory.h>
#include <Graphics/GraphicsPipelineSystem.h>

#include <Resource/Mesh.h>
#include <ECS/MeshRenderer.h>

namespace lyra {

// cubemap base class
class CubemapBase : public vulkan::Image, public vulkan::GPUMemory, public GraphicsPipeline {
public:	
	CubemapBase(
		const lsd::Array<lsd::StringView, 6>& paths,
		lsd::StringView vertexShaderPath,
		lsd::StringView fragShaderPath,
		Camera* const camera,
		const Image::Format& format,
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
	NODISCARD constexpr VkDescriptorImageInfo getDescriptorcubemapInfo(const Image::Layout& layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) const noexcept {
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

	lsd::Vector<vulkan::Descriptor> m_descriptorSets;
	Mesh m_cubeMesh;
	MeshRenderer m_cubeMeshRenderer;
};

// cubemap
class Cubemap : public CubemapBase {
public:
	Cubemap(
		const lsd::Array<lsd::StringView, 6>& paths,
		Script* script,
		Camera* const camera,
		const Image::Format& format = VK_FORMAT_R8G8B8A8_SRGB,
		const ColorBlending& colorBlending = ColorBlending::BLEND_ENABLE,
		const Tessellation& tessellation = Tessellation::TESSELLATION_ENABLE,
		const Multisampling& multisampling = Multisampling::MULTISAMPLING_ENABLE
	) : CubemapBase(paths, "data/shader/cubemapVert.spv", "data/shader/cubemapFrag.spv", camera, format, colorBlending, tessellation, multisampling) { }
};

// skybox
class Skybox : public CubemapBase {
public:
	Skybox(
		const lsd::Array<lsd::StringView, 6>& paths,
		Camera* const camera,
		const Image::Format& format = VK_FORMAT_R8G8B8A8_SRGB,
		const ColorBlending& colorBlending = ColorBlending::BLEND_ENABLE,
		const Tessellation& tessellation = Tessellation::TESSELLATION_ENABLE,
		const Multisampling& multisampling = Multisampling::MULTISAMPLING_ENABLE
	) : CubemapBase(paths, "data/shader/skyboxVert.spv", "data/shader/skyboxFrag.spv", camera, format, colorBlending, tessellation, multisampling) 
	{ camera->m_skybox = this; }
};

} // namespace lyra
