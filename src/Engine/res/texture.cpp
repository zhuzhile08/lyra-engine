#include <res/texture.h>

namespace lyra {

Texture::Texture() { }

void Texture::destroy() {
    var.image.destroy();

    delete device;
    delete renderer;
}

void Texture::create(VulkanDevice device, Renderer renderer, str path, VkFormat format, int channelsToLoad) {
    this->device = &device;
    this->renderer = &renderer;

    // load the image
    int                     width, height, channels;
    stbi_uc* imagePixelData = stbi_load(path, &width, &height, &channels, channelsToLoad);
    if (imagePixelData == nullptr) LOG_ERROR("Failed to load image from path: ", path, " at: ", GET_ADDRESS(this), "!")

    // create a staging buffer
    VulkanGPUBuffer         stagingBuffer;
    VkDeviceSize imageMemSize = width * height * 4; /// @todo I don't know, the tutorial said 4 bytes per pixel, but T'm not to sure about it. Probably will make it a bit more dynamic
    stagingBuffer.create(device, imageMemSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

    // copy the image data into the staging buffer
    void*                   data;
    void*                   imagePtr = imagePixelData;
    stagingBuffer.map_memory(data);
        memcpy(data, imagePtr, static_cast<size_t>(imageMemSize));
    stagingBuffer.unmap_memory();

    // image creation info
    auto                    imageCreationInfo = var.image.get_image_create_info(format, {static_cast<uint32>(width), static_cast<uint32>(height), 1}, 
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    // memory allocation info
    VmaAllocationCreateInfo memoryAllocInfo = {0, VMA_MEMORY_USAGE_GPU_ONLY};
    // create the image and allocate its memory
    vmaCreateImage(device.get().allocator, &imageCreationInfo, &memoryAllocInfo, &var.image.image, &var.memory, nullptr);

    // convert the image layout and copy it from the buffer
    copy_from_buffer(stagingBuffer, {static_cast<uint32>(width), static_cast<uint32>(height), 1});

    // create the image view
    var.image.create_view(device, format, {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

    // free the pixels of the image
    stbi_image_free(imagePixelData);
    // destroy the staging buffer
    stagingBuffer.destroy();
}

void Texture::create(str path, VkFormat format, int channelsToLoad) {
    // destroy the old image
    var.image.destroy();

    // load the image
    int                     width, height, channels;
    stbi_uc* imagePixelData = stbi_load(path, &width, &height, &channels, channelsToLoad);
    if (imagePixelData == nullptr) LOG_ERROR("Failed to load image from path: ", path, " at: ", GET_ADDRESS(this), "!")

    // create a staging buffer
    VulkanGPUBuffer         stagingBuffer;
    VkDeviceSize imageMemSize = width * height * 4; /// @todo I don't know, the tutorial said 4 bytes per pixel, but T'm not to sure about it. Probably will make it a bit more dynamic
    stagingBuffer.create(*device, imageMemSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

    // copy the image data into the staging buffer
    void*                   data;
    void*                   imagePtr = imagePixelData;
    stagingBuffer.map_memory(data);
        memcpy(data, imagePtr, static_cast<size_t>(imageMemSize));
    stagingBuffer.unmap_memory();

    // image creation info
    auto                    imageCreationInfo = var.image.get_image_create_info(format, {static_cast<uint32>(width), static_cast<uint32>(height), 1}, 
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    // memory allocation info
    VmaAllocationCreateInfo memoryAllocInfo = {0, VMA_MEMORY_USAGE_GPU_ONLY};
    // create the image and allocate its memory
    vmaCreateImage(device->get().allocator, &imageCreationInfo, &memoryAllocInfo, &var.image.image, &var.memory, nullptr);

    // convert the image layout and copy it from the buffer
    copy_from_buffer(stagingBuffer, {static_cast<uint32>(width), static_cast<uint32>(height), 1});

    // create the image view
    var.image.create_view(*device, format, {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

    // free the pixels of the image
    stbi_image_free(imagePixelData);
    // destroy the staging buffer
    stagingBuffer.destroy();
}

void Texture::copy_from_buffer(VulkanGPUBuffer stagingBuffer, VkExtent3D extent, VkFormat Format) {
    // temporary command buffer for copying
    VulkanCommandBuffer     cmdBuff;
    // begin recording
    cmdBuff.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    // image memory barrier to transition to the copyable barrier
    VkImageMemoryBarrier transitionToBarrier = {
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        nullptr,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        0,
        0,
        var.image.image,
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
    };

    vkCmdPipelineBarrier(cmdBuff.get(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &transitionToBarrier);

    // copy image in buffer to the image
    VkBufferImageCopy imageCopy = {
        0,
        0,
        0,
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 0},
        {},
        extent
    };

    vkCmdCopyBufferToImage(cmdBuff.get(), stagingBuffer.get().buffer, var.image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

    // convert the image layout back to the normal
    VkImageMemoryBarrier transitionBackBarrier = {
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        nullptr,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_SHADER_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        0,
        0,
        var.image.image,
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
    };

    vkCmdPipelineBarrier(cmdBuff.get(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &transitionBackBarrier);

    // end recording
    cmdBuff.end();

    // submit queues after recording
    renderer->submit_queue(device->get().graphicsQueue, cmdBuff);
    renderer->wait_queue(device->get().graphicsQueue);

    // destroy command buffer
    cmdBuff.destroy();
}

} // namespace lyra
