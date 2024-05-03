/*************************
 * @file texture.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief texture class
 * 
 * @date 2022-31-7
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <Common/Common.h>

#include <Graphics/VulkanRenderSystem.h>

#include <Resource/LoadTextureFile.h>

namespace lyra {

// Textures and images
class Texture {
public:
	enum class Type {
		texture = 0,
		normalMap = 1,
		lightMap = 2,
		lightMapDirectional = 3,
		shadowMask = 4
	};

	Texture() = default;
	Texture(const resource::TextureFile& imageData, vulkan::Image::Format format = vulkan::Image::Format::r8g8b8a8SRGB);

	NODISCARD constexpr VkDescriptorImageInfo getDescriptorImageInfo(vulkan::Image::Layout layout = vulkan::Image::Layout::shaderReadOnly) const noexcept {
		return {
			m_sampler,
			m_imageResource.view,
			static_cast<VkImageLayout>(layout)
		};
	}

	NODISCARD constexpr const vulkan::vk::Sampler& sampler() const noexcept { return m_sampler; }

private:
	vulkan::Image m_image;
	vulkan::Image::Resource m_imageResource;
	vulkan::GPUMemory m_memory;
	vulkan::vk::Sampler m_sampler;

	Type m_type;

	uint32 m_width;
	uint32 m_height;

	void createSampler(
		const resource::TextureFile& imageData,
		const VkFilter& magnifiedTexel = VK_FILTER_LINEAR,
		const VkFilter& minimizedTexel = VK_FILTER_LINEAR,
		const VkSamplerMipmapMode& mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR
	);

	void generate_mipmaps() const;
};

} // namespace lyra
