/*************************
 * @file   texture.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief  texture class
 * 
 * @date   2022-31-7
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <Common/Common.h>

#include <Application/Application.h>

#include <Resource/LoadTexture.h>

#include <Graphics/VulkanImpl/GPUMemory.h>
#include <Graphics/VulkanImpl/GPUBuffer.h>
#include <Graphics/VulkanImpl/CommandBuffer.h>
#include <Graphics/VulkanImpl/Image.h>

#include <algorithm>
#include <vulkan/vulkan.h>

namespace lyra {

// Textures and images
class Texture : public vulkan::Image, public vulkan::GPUMemory {
public:
	// type of the image
	enum Type : unsigned int {
		// use the image as a texture/sprite
		TYPE_TEXTURE = 0,
		// use the image as a normal map
		TYPE_NORMAL_MAP = 1,
		// use the image as a lightmap
		TYPE_LIGHTMAP = 2,
		// load the image as a directional lightmap
		TYPE_DIRECTIONAL_LIGHTMAP = 3,
		// load the image as a shadow mask
		TYPE_SHADOW_MASK = 4
	};

	// how to treat the alpha value of the image
	enum Alpha : unsigned int {
		ALPHA_TRANSPARENT = 1,
		ALPHA_BLACK = 3,
		ALPHA_WHITE = 5
	};

	// how the UVs should read the image
	enum Dimension : unsigned int {
		// one dimensional image
		DIMENSION_1D = 0,
		// two dimensional image
		DIMENSION_2D = 1,
		// three dimensional image
		DIMENSION_3D = 2
	};

	// how to wrap the image if the UVs exceeds the border of the image
	enum Wrap : unsigned int {
		// repeat the image
		WRAP_REPEAT = 0,
		// repeat the image whilst mirroring it
		WRAP_MIRROR = 1,
		// don't wrap or clamp the image at all
		WRAP_ONCE = 2,
		// clamp the image to the border
		WRAP_CLAMP = 3,
	};

	// anistropic filtering
	enum Anistropy : unsigned int {
		// enable anistropic filtering
		ANISTROPY_DISABLE = 0U,
		// disable anistropic filtering
		ANISTROPY_ENABLE = 1U
	};
	/**
	 * @brief create the texture and the sampler
	 *
	 * @param imageData image data and information
	 * @param format format of the image
	 */
	Texture(const util::detail::LoadedTexture& imageData, const VkFormat& format = VK_FORMAT_R8G8B8A8_SRGB);

	/**
	 * @brief get the information to bind to a descriptor
	 * 
	 * @param layout image layout
	 * 
	 * @return VkDescriptorImageInfo
	*/
	NODISCARD constexpr VkDescriptorImageInfo getDescriptorImageInfo(const VkImageLayout& layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) const noexcept {
		return {
			m_sampler,
			m_view,
			layout
		};
	}

	/**
	 * @brief get the sampler
	 * 
	 * @return const lyra::vulkan::vk::Sampler&
	*/
	NODISCARD constexpr const vulkan::vk::Sampler& sampler() const noexcept { return m_sampler; }

private:
	vulkan::vk::Sampler m_sampler;
	uint32 m_width;
	uint32 m_height;
	uint32 m_mipmap;

	/**
	 * @brief create the image sampler
	 *
	 * @param imageData texture information
	 * @param magnifiedTexel how to filter the image if a pixel is smaller than a texel
	 * @param minimizedTexel how to filter the image if a pixel is bigger than a texel
	 * @param mipmapMode the mode of mipmapping
	 */
	void createSampler(
		const util::detail::LoadedTexture& imageData,
		const VkFilter& magnifiedTexel = VK_FILTER_LINEAR,
		const VkFilter& minimizedTexel = VK_FILTER_LINEAR,
		const VkSamplerMipmapMode& mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR
	);

	/**
	 * @brief generate the mipmaps for the image
	*/
	void generate_mipmaps() const;
};

} // namespace lyra
