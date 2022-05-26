#pragma once

#include <lyra.h>
#include <core/defines.h>
#include <core/logger.h>
#include <core/rendering/vulkan/GPU_memory.h>
#include <core/rendering/vulkan/GPU_buffer.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/vulkan/vulkan_image.h>
#include <graphics/renderer.h>

#include <algorithm>
#include <stb_image.h>
#include <vulkan/vulkan.h>

namespace lyra {

// Textures and images
class Texture : private VulkanImage, private VulkanGPUMemory {
public:
	// type of the image
	enum class Type : int {
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
	enum class Alpha : int {
		ALPHA_TRANSPARENT = 1,
		ALPHA_BLACK = 3,
		ALPHA_WHITE = 5
	};

	// how the UVs should read the image
	enum class Dimension : int {
		// one dimensional image
		DIMENSION_1D = 0,
		// two dimensional image
		DIMENSION_2D = 1,
		// three dimensional image
		DIMENSION_3D = 2
	};

	// how to wrap the image if the UVs exceeds the border of the image
	enum class Wrap : int {
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
	enum class Anistropy : unsigned int {
		// enable anistropic filtering
		ANISTROPY_DISABLE = 0U,
		// disable anistropic filtering
		ANISTROPY_ENABLE = 1U
	};

	struct CreateInfo {
		// path of image
		const std::string path;
		// type of the image
		const Type type = Type::TYPE_TEXTURE;
		// how to treat the alpha value of the image
		const Alpha alpha = Alpha::ALPHA_BLACK;
		// how the UVs should read the image
		const Dimension dimension = Dimension::DIMENSION_2D;
		// how to wrap the image if the UVs exceeds the border of the image
		const Wrap wrap = Wrap::WRAP_REPEAT;
		// anistropic filtering
		const Anistropy anistropy = Anistropy::ANISTROPY_ENABLE;
	};

	Texture();

	/**
	 * @brief destructor of the texture
	 */
	virtual ~Texture() noexcept;

	/**
	 * @brief destroy the texture
	 */
	void destroy() noexcept;

	Texture operator=(const Texture&) const noexcept = delete;

	/**
	 * @brief create the texture and the sampler
	 *
	 * @param info creation information
	 * @param format format of the image
	 */
	void create(const CreateInfo info, const VkFormat format = VK_FORMAT_R8G8B8A8_SRGB);

	/**
	 * @brief get the information to bind to a descriptor
	 * 
	 * @return const VkDescriptorImageInfo
	*/
	[[nodiscard]] const VkDescriptorImageInfo get_descriptor_image_info() const noexcept;

	/**
	 * @brief get the image
	 * 
	 * @return const lyra::VulkanImage&
	*/
	[[nodiscard]] const VkImageView& view() const noexcept { return _view; }
	/**
	 * @brief get the sampler
	 * 
	 * @return const VkSampler&
	*/
	[[nodiscard]] const VkSampler& sampler() const noexcept { return _sampler; }
	/**
	 * @brief get the memory
	 * 
	 * @return const VmaAllocation&
	*/
	[[nodiscard]] const VmaAllocation& memory() const noexcept { return _memory; }

	[[nodiscard]] const std::string get_path() const noexcept { return _path; }

private:
	VkSampler _sampler = VK_NULL_HANDLE;
	uint32 _width;
	uint32 _height;
	uint32 _mipmap;
	std::string _path;

	/**
	 * @brief copy raw image data from a buffer into the image
	 *
	 * @param stagingBuffer buffer
	 * @param extent size of the image
	 */
	void copy_from_buffer(const VulkanGPUBuffer* stagingBuffer, const VkExtent3D extent);

	/**
	 * load a image from a path
	 *
	 * @param info creation information
	 * @param format format of the image
	 */
	void load_image(const CreateInfo info, const VkFormat format = VK_FORMAT_R8G8B8A8_SRGB);

	/**
	 * @brief create the image sampler
	 *
	 * @param extendedTexels how to render the image if the surface is bigger than the image
	 * @param magnifiedTexel how to filter the image if a pixel is smaller than a texel
	 * @param minimizedTexel how to filter the image if a pixel is bigger than a texel
	 * @param mipmapMode the mode of midmapping
	 */
	void create_sampler(
		const CreateInfo info,
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
