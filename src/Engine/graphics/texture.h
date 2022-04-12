#pragma once

#include <core/defines.h>
#include <core/logger.h>
#include <core/rendering/vulkan/descriptor.h>
#include <core/rendering/vulkan/vulkan_image.h>
#include <graphics/GPU_buffer.h>
#include <core/rendering/vulkan/GPU_memory.h>
#include <core/rendering/context.h>
#include <graphics/renderer.h>

#include <stb_image.h>
#include <vulkan/vulkan.h>

namespace lyra {

class Texture : private VulkanImage, private VulkanGPUMemory {
public:
	Texture();

	/**
	 * @brief destroy the texture
	 */
	void destroy() noexcept;

	/**
	 * @brief create the texture and the sampler
	 *
	 * @param context context
	 * @param path path of the image
	 * @param format format of the image
	 * @param channelsToLoad what channels to load
	 */
	void create(const Context* context, str path, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, int channelsToLoad = STBI_rgb_alpha);
	/**
	 * @brief load a new texture into the image
	 *
	 * @param path path of the new image
	 * @param format format of the image
	 * @param channelsToLoad what channels to load
	 */
	void create(str path, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, int channelsToLoad = STBI_rgb_alpha);

	/**
	 * @brief draw the texture
	 *
	 * @renderer the subrenderer to render it on
	 */
	void bind(Renderer renderer);

	/**
	const VulkanImage image() const noexcept { return _image; }
	const VkSampler sampler() const noexcept { return _sampler; }
	const VmaAllocation memory() const noexcept { return _memory; }
	const VulkanDescriptor descriptor() const noexcept { return _descriptor; }
	*/

private:
	VkSampler _sampler = VK_NULL_HANDLE;
	VulkanDescriptor _descriptor;

	const Context* context;

	/**
	 * @brief copy raw image data from a buffer into the image
	 *
	 * @param stagingBuffer buffer
	 * @param extent size of the image
	 */
	void copy_from_buffer(VulkanGPUBuffer stagingBuffer, VkExtent3D extent);

	/**
	 * load a image from a path
	 *
	 * @param path path of the new image
	 * @param format format of the image
	 * @param channelsToLoad what channels to load
	 */
	void load_image(str path, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, int channelsToLoad = STBI_rgb_alpha);
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
		VkFilter magnifiedTexel = VK_FILTER_LINEAR,
		VkFilter minimizedTexel = VK_FILTER_LINEAR,
		VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
		VkSamplerAddressMode extendedTexels = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		VkBool32 anisotropy = VK_TRUE
	);
};

} // namespace lyra
