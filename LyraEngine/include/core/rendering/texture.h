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

#include <core/application.h>
#include <core/util.h>
#include <core/logger.h>
#include <core/rendering/vulkan/GPU_memory.h>
#include <core/rendering/vulkan/GPU_buffer.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/vulkan/vulkan_image.h>
#include <core/rendering/assets.h>

#include <algorithm>
#include <lz4.h>
#include <vulkan/vulkan.h>

namespace lyra {

// Textures and images
class Texture : private vulkan::Image, private vulkan::GPUMemory {
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

	Texture() { }
	/**
	 * @brief create the texture and the sampler
	 *
	 * @param path path
	 * @param format format of the image
	 */
	Texture(const char* path, const VkFormat format = VK_FORMAT_R8G8B8A8_SRGB);

	/**
	 * @brief destructor of the texture
	 */
	virtual ~Texture() noexcept;
	/**
	 * @brief destroy the texture
	 */
	void destroy() noexcept {
		this->~Texture();
	}

	Texture operator=(const Texture&) const noexcept = delete;

	/**
	 * @brief get the information to bind to a descriptor
	 * 
	 * @return const VkDescriptorImageInfo
	*/
	NODISCARD const VkDescriptorImageInfo get_descriptor_image_info() const noexcept {
		return {
			m_sampler,
			m_view,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};
	}

	/**
	 * @brief get the image
	 * 
	 * @return const lyra::vulkan::Image&
	*/
	NODISCARD const VkImageView& view() const noexcept { return m_view; }
	/**
	 * @brief get the sampler
	 * 
	 * @return const VkSampler&
	*/
	NODISCARD const VkSampler& sampler() const noexcept { return m_sampler; }
	/**
	 * @brief get the memory
	 * 
	 * @return const VmaAllocation&
	*/
	NODISCARD const VmaAllocation& memory() const noexcept { return m_memory; }
	/**
	* @brief get the path of the image
	* 
	* @return const char* const
	**/
	NODISCARD const char* const path() const noexcept { return m_path; }

private:
	VkSampler m_sampler = VK_NULL_HANDLE;
	uint32 m_width;
	uint32 m_height;
	uint32 m_mipmap;
	const char* m_path;

	/**
	 * @brief copy raw image data from a buffer into the image
	 *
	 * @param stagingBuffer buffer
	 * @param extent size of the image
	 */
	void copy_from_buffer(const vulkan::GPUBuffer* stagingBuffer, const VkExtent3D extent);

	/**
	 * load a image from a path
	 *
	 * @param textureInfo texture information
	 * @param format format of the image
	 */
	void load_image(Assets::TextureInfo& textureInfo, const VkFormat format = VK_FORMAT_R8G8B8A8_SRGB);

	/**
	 * @brief create the image sampler
	 *
	 * @param textureInfo texture information
	 * @param magnifiedTexel how to filter the image if a pixel is smaller than a texel
	 * @param minimizedTexel how to filter the image if a pixel is bigger than a texel
	 * @param mipmapMode the mode of mipmapping
	 */
	void create_sampler(
		Assets::TextureInfo& textureInfo,
		const VkFilter magnifiedTexel = VK_FILTER_LINEAR,
		const VkFilter minimizedTexel = VK_FILTER_LINEAR,
		const VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR
	);

	/**
	 * @brief generate the mipmaps for the image
	*/
	void generate_mipmaps() const;
};

} // namespace lyra
