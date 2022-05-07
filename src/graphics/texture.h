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

class Texture : private VulkanImage, private VulkanGPUMemory {
public:
	struct CreateInfo {

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
	 * @param path path of the image
	 * @param format format of the image
	 * @param channelsToLoad what channels to load
	 */
	void create(const std::string path, const VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, const int channelsToLoad = STBI_rgb_alpha);

	/**
	 * @brief get the information to bind to a descriptor
	 * 
	 * @return const VkDescriptorImageInfo
	*/
	[[nodiscard]] const VkDescriptorImageInfo get_descriptor_image_info() const noexcept;

	/**
	 * @brief get the image
	 * 
	 * @return const lyra::VulkanImage
	*/
	[[nodiscard]] const VkImageView view() const noexcept { return _view; }
	/**
	 * @brief get the sampler
	 * 
	 * @return const VkSampler
	*/
	[[nodiscard]] const VkSampler sampler() const noexcept { return _sampler; }
	/**
	 * @brief get the memory
	 * 
	 * @return const VmaAllocation
	*/
	[[nodiscard]] const VmaAllocation memory() const noexcept { return _memory; }

private:
	VkSampler _sampler = VK_NULL_HANDLE;
	uint32 _width;
	uint32 _height;
	uint32 _mipmap;

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
	 * @param path path of the new image
	 * @param format format of the image
	 * @param channelsToLoad what channels to load
	 */
	void load_image(const std::string path, const VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, const int channelsToLoad = STBI_rgb_alpha);

	/**
	 * @brief create the image sampler
	 *
	 * @param magnifiedTexel how to filter the image if a pixel is smaller than a texel
	 * @param minimizedTexel how to filter the image if a pixel is bigger than a texel
	 * @param mipmapMode the mode of midmapping
	 * @param extendedTexels how to render the image if the surface is bigger than the image
	 * @param anistropy how further distances are filtered
	 */
	void create_sampler(
		const VkFilter magnifiedTexel = VK_FILTER_LINEAR,
		const VkFilter minimizedTexel = VK_FILTER_LINEAR,
		const VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		const VkSamplerAddressMode extendedTexels = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		const VkBool32 anisotropy = VK_TRUE
	);

	/**
	 * @brief generate the mipmaps for the image
	*/
	void generate_mipmaps() const;
};

} // namespace lyra
