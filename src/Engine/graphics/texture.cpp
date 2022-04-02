#include <graphics/texture.h>

namespace lyra {

Texture::Texture() { }

void Texture::destroy() noexcept {
	destroy_view();
	vkDestroySampler(context->device().device(), _sampler, nullptr);
}

void Texture::create(const Context* context, str path, VkFormat format, int channelsToLoad) {
	LOG_INFO("Creating Vulkan texture and image sampler... ")

	this->context = context;

	load_image(path, format, channelsToLoad);
	create_sampler();

	// create the descriptor set
	VulkanDescriptor::Writer writer;
	writer.add_image_write({ _sampler, _view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
	_descriptor.create(&context->device(), context->descriptorSetLayout(), context->descriptorPool(), writer);

	LOG_INFO("Successfully created Vulkan texture with path: ", path, " with image sampler at: ", GET_ADDRESS(this))
}

void Texture::create(str path, VkFormat format, int channelsToLoad) {
	LOG_INFO("Recreating Vulkan texture and image sampler... ")

	destroy();

	load_image(path, format, channelsToLoad);
	create_sampler();

	// create the descriptor set
	VulkanDescriptor::Writer writer;
	writer.add_image_write({ _sampler, _view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
	_descriptor.create(&context->device(), context->descriptorSetLayout(), context->descriptorPool(), writer);

	LOG_INFO("Successfully recreated Vulkan texture with path: ", path, " with image sampler at: ", GET_ADDRESS(this))
}

void Texture::copy_from_buffer(VulkanGPUBuffer stagingBuffer, VkExtent3D extent) {
	// temporary command buffer for copying
	VulkanCommandBuffer     cmdBuff;
	cmdBuff.create(&context->device(), &context->commandPool());
	// begin recording
	cmdBuff.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// copy image in buffer to the image
	VkBufferImageCopy imageCopy = {
		0,
		0,
		0,
		{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
		{0, 0, 0},
		extent
	};

	vkCmdCopyBufferToImage(cmdBuff.get(), stagingBuffer.buffer(), _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

	// end recording
	cmdBuff.end();

	// submit queues after recording
	cmdBuff.submit_queue(context->device().graphicsQueue().queue);
	cmdBuff.wait_queue(context->device().graphicsQueue().queue);

	// destroy command buffer
	cmdBuff.destroy();
}

void Texture::bind(Renderer renderer) {
	renderer._bind_queue.add([&]() {renderer.bind_descriptor(_descriptor); });
}

void Texture::load_image(str path, VkFormat format, int channelsToLoad) {
	// load the image
	int                     width, height, channels;
	stbi_uc* imagePixelData = stbi_load(path, &width, &height, &channels, channelsToLoad);
	if (imagePixelData == nullptr) LOG_ERROR("Failed to load image from path: ", path, " at: ", GET_ADDRESS(this), "!")

	// create a staging buffer
	VulkanGPUBuffer         stagingBuffer;
	VkDeviceSize imageMemSize = width * height * 4; /// @todo I don't know, the tutorial said 4 bytes per pixel, but T'm not to sure about it. Probably will make it a bit more dynamic
	stagingBuffer.create(&context->device(), imageMemSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	// copy the image data into the staging buffer
	void* imagePtr = imagePixelData;
	stagingBuffer.copy_data(imagePtr);

	// image creation info
	auto                    imageCreationInfo = get_image_create_info(format, { static_cast<uint32>(width), static_cast<uint32>(height), 1 },
		VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	// memory allocation info
	VmaAllocationCreateInfo memoryAllocInfo{ 0, VMA_MEMORY_USAGE_GPU_ONLY };
	// create the image and allocate its memory
	if (vmaCreateImage(context->device().allocator(), &imageCreationInfo, &memoryAllocInfo, &_image, &_memory, nullptr) != VK_SUCCESS) LOG_ERROR("Failed to load image from path: ", path)

	// convert the image layout and copy it from the buffer
	transition_layout(context->device(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, context->commandPool());
	copy_from_buffer(stagingBuffer, { static_cast<uint32>(width), static_cast<uint32>(height), 1 });
	transition_layout(context->device(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, context->commandPool());

	// create the image view
	create_view(&context->device(), format, { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

	// free the pixels of the image
	stbi_image_free(imagePixelData);
	// destroy the staging buffer
	stagingBuffer.destroy();
}

void Texture::create_sampler(VkFilter magnifiedTexel, VkFilter minimizedTexel, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode extendedTexels, VkBool32 anisotropy) {
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(context->device().physicalDevice(), &properties);

	VkSamplerCreateInfo samplerInfo{
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		nullptr,
		0,
		magnifiedTexel,
		minimizedTexel,
		mipmapMode,
		extendedTexels,
		extendedTexels,
		extendedTexels,
		0.0f,
		anisotropy,
		properties.limits.maxSamplerAnisotropy,
		VK_FALSE,
		VK_COMPARE_OP_NEVER,
		0.0f,
		0.0f,
		VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		VK_FALSE
	};

	if (vkCreateSampler(context->device().device(), &samplerInfo, nullptr, &_sampler) != VK_SUCCESS) LOG_EXEPTION("Failed to create Vulkan image sampler!");
}

} // namespace lyra
