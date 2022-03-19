#pragma once

#include <core/defines.h>
#include <core/logger.h>
#include <rendering/vulkan_wrappers.h>
#include <rendering/renderer.h>

#include <stb_image.h>
#include <vulkan/vulkan.h>

namespace lyra {

class Texture {
private:
    /**
     * @brief struct holding all the variables
     */
    struct          Variables {
        VulkanImage     image;
        VkSampler       sampler;
        VmaAllocation   memory;
    };

public:
    Texture();

    /**
     * @brief destroy the texture
     */
    void            destroy();

    /**
     * @brief create the texture and the sampler
     * 
     * @param device device
     * @param renderer renderer
     * @param path path of the image
     * @param format format of the image
     * @param channelsToLoad what channels to load
     */
    void            create(VulkanDevice device, Renderer renderer, str path, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, int channelsToLoad = STBI_rgb_alpha);
    /**
     * @brief load a new texture into the image
     * 
     * @param path path of the new image
     * @param format format of the image
     * @param channelsToLoad what channels to load
     */
    void            create(str path, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, int channelsToLoad = STBI_rgb_alpha);

private:
    Variables       var;

    VulkanDevice*   device;
    Renderer*       renderer;

    /**
     * @brief copy raw image data from a buffer into the image
     * 
     * @param renderer renderer
     * @param stagingBuffer buffer
     * @param extent size of the image
     * @param Format format of the image
     */
    void            copy_from_buffer(VulkanGPUBuffer stagingBuffer, VkExtent3D extent, VkFormat Format = VK_FORMAT_R8G8B8A8_SRGB);

    /**
     * load a image from a path
     * 
     * @param path path of the new image
     * @param format format of the image
     * @param channelsToLoad what channels to load
     */
    void            load_image(str path, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, int channelsToLoad = STBI_rgb_alpha);

    void            create_sampler(
        VkFilter                magnifiedTexel  = VK_FILTER_LINEAR, 
        VkFilter                minimizedTexel  = VK_FILTER_LINEAR, 
        VkSamplerMipmapMode     mipmapMode      = VK_SAMPLER_MIPMAP_MODE_NEAREST,
        VkSamplerAddressMode    extendedTexels  = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        VkBool32                anisotropy      = VK_TRUE
    );
};

} // namespace lyra
