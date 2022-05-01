#include <graphics/texture.h>

namespace lyra {

Texture::Texture() { }

Texture::~Texture() noexcept {
	vkDestroySampler(Application::context()->device()->device(), _sampler, nullptr);

	LOG_INFO("Successfully destroyed Texture!");
}

void Texture::destroy() noexcept {
	this->~Texture();
}

void Texture::create(const string path, const VkFormat format, const int channelsToLoad) {
	LOG_INFO("Creating Vulkan texture and image sampler... ");

	LOG_DEBUG(TAB, "path: ", path);

	load_image(path, format, channelsToLoad);
	create_sampler();

	LOG_INFO("Successfully created Vulkan texture with path: ", path, " with image sampler at: ", get_address(this), END_L);
}

void Texture::load_image(const string path, const VkFormat format, int channelsToLoad) {
	// load the image
	int width, height, channels;
	stbi_uc* imagePixelData = stbi_load(path, &width, &height, &channels, channelsToLoad);
	if (imagePixelData == nullptr) LOG_ERROR("Failed to load image from path: ", path, " at: ", get_address(this), "!");

	_width = width; _height = height;

	LOG_DEBUG(TAB, "width: ", width, " and height: ", height);

	// calculate the mipmap levels of the image
	_mipmap = static_cast<uint32>(std::max(static_cast<int>(std::floor(std::log2(std::max(width, height)))) - 3, 1)); // since the last few are too small to be what I would consider useful, I'm subtracting it

	LOG_DEBUG(TAB, "midmapping levels: ", _mipmap);

	// create a staging buffer
	VulkanGPUBuffer stagingBuffer;
	VkDeviceSize imageMemSize = static_cast<uint64>(width * height * 4); /// @todo I don't know, the tutorial said 4 bytes per pixel, but T'm not to sure about it. Probably will make it a bit more dynamic
	stagingBuffer.create(Application::context()->device(), imageMemSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	// copy the image data into the staging buffer
	void* imagePtr = imagePixelData;
	stagingBuffer.copy_data(imagePtr);

	// create the image and allocate its memory
	if (vmaCreateImage(
		Application::context()->device()->allocator(), 
		&get_image_create_info(
			format, 
			{ static_cast<uint32>(width), static_cast<uint32>(height), 1 },
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			_mipmap
		),
		&get_alloc_create_info(VMA_MEMORY_USAGE_GPU_ONLY),
		& _image, 
		& _memory, 
		nullptr
	) != VK_SUCCESS) LOG_ERROR("Failed to load image from path: ", path);

	// convert the image layout and copy it from the buffer
	transition_layout(Application::context()->device(), Application::context()->commandPool(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_FORMAT_R8G8B8A8_SRGB, { VK_IMAGE_ASPECT_COLOR_BIT, 0, _mipmap, 0, 1 });
	copy_from_buffer(&stagingBuffer, { static_cast<uint32>(width), static_cast<uint32>(height), 1 });
	// generate the mipmaps
	generate_mipmaps();

	// create the image view
	create_view(format, { VK_IMAGE_ASPECT_COLOR_BIT, 0, _mipmap, 0, 1 });

	// free the pixels of the image
	stbi_image_free(imagePixelData);
}

void Texture::create_sampler(const VkFilter magnifiedTexel, const VkFilter minimizedTexel, const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode extendedTexels, const VkBool32 anisotropy) {
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(Application::context()->device()->physicalDevice(), &properties);

	VkSamplerCreateInfo samplerInfo {
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
		static_cast<float>(_mipmap),
		VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		VK_FALSE
	};

	if (vkCreateSampler(Application::context()->device()->device(), &samplerInfo, nullptr, &_sampler) != VK_SUCCESS) LOG_EXEPTION("Failed to create Vulkan image sampler!");

	LOG_DEBUG(TAB, "Created image sampler at: ", get_address(this));
}

void Texture::generate_mipmaps() const {
	// check if image supports linear filtering
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(Application::context()->device()->physicalDevice(), VK_FORMAT_R8G8B8A8_SRGB, &formatProperties);
	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) LOG_EXEPTION("Image does not support linear filtering with its current format!", END_L);

	// temporary command buffer for generating midmaps
	VulkanCommandBuffer     cmdBuff;
	cmdBuff.create(Application::context()->device(), Application::context()->commandPool());
	// begin recording
	cmdBuff.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	int32 mipWidth = _width, mipHeight = _height;

	for (uint32 i = 1; i < _mipmap; i++) {
		cmdBuff.pipeline_barrier(
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT, 
			nullptr, 
			nullptr, 
			&get_image_memory_barrier(
				VK_ACCESS_TRANSFER_WRITE_BIT,
				VK_ACCESS_TRANSFER_READ_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
				{ VK_IMAGE_ASPECT_COLOR_BIT, i - 1, 1, 0, 1 }
			)
		);

		// draw the mipmap
		VkImageBlit blit {
			{ VK_IMAGE_ASPECT_COLOR_BIT, i - 1, 0, 1},
			{ { 0, 0, 0 }, { mipWidth, mipHeight, 1 } },
			{ VK_IMAGE_ASPECT_COLOR_BIT, i, 0, 1},
			{ { 0, 0, 0 }, { (mipWidth > 1) ? mipWidth / 2 : 1, (mipHeight > 1) ? mipHeight / 2 : 1, 1 } }
		};

		vkCmdBlitImage(cmdBuff.get(), _image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

		cmdBuff.pipeline_barrier(
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			nullptr,
			nullptr,
			&get_image_memory_barrier(
				VK_ACCESS_TRANSFER_READ_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				{ VK_IMAGE_ASPECT_COLOR_BIT, i - 1, 1, 0, 1 }
			)
		);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	cmdBuff.pipeline_barrier(
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		nullptr,
		nullptr,
		&get_image_memory_barrier(
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_SHADER_READ_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			{ VK_IMAGE_ASPECT_COLOR_BIT, _mipmap - 1, 1, 0, 1 }
		)
	);

	// end recording
	cmdBuff.end();

	// submit queues after recording
	cmdBuff.submit_queue(Application::context()->device()->graphicsQueue().queue);
	cmdBuff.wait_queue(Application::context()->device()->graphicsQueue().queue);

	LOG_DEBUG(TAB, "Created image mipmaps!");
}

void Texture::copy_from_buffer(const VulkanGPUBuffer* stagingBuffer, const VkExtent3D extent) {
	// temporary command buffer for copying
	VulkanCommandBuffer     cmdBuff;
	cmdBuff.create(Application::context()->device(), Application::context()->commandPool());
	// begin recording
	cmdBuff.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// copy image in buffer to the image
	VkBufferImageCopy imageCopy{
		0,
		0,
		0,
		{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
		{0, 0, 0},
		extent
	};

	vkCmdCopyBufferToImage(cmdBuff.get(), stagingBuffer->buffer(), _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

	// end recording
	cmdBuff.end();

	// submit queues after recording
	cmdBuff.submit_queue(Application::context()->device()->graphicsQueue().queue);
	cmdBuff.wait_queue(Application::context()->device()->graphicsQueue().queue);

	LOG_DEBUG(TAB, "Copied image data from buffer to image at: ", get_address(this));
}

const VkDescriptorImageInfo Texture::get_descriptor_image_info() const noexcept {
	return {
		_sampler,
		_view,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	};
}

} // namespace lyra
