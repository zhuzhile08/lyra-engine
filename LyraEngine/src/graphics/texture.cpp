#include <graphics/texture.h>

namespace lyra {

void Texture::create(char* const path, const VkFormat format) {
	Logger::log_info("Creating Vulkan texture and image sampler... ");

	AssetManager::TextureInfo textureInfo;
	textureInfo = AssetManager::unpack_texture(path);

	// assign some variables
	_width = textureInfo.width; 
	_height = textureInfo.height;
	_mipmap = textureInfo.mipmap;
	_path = path;

	load_image(textureInfo, format);
	create_sampler(textureInfo);

	Logger::log_info("Successfully created Vulkan texture with path: ", path, " with image sampler at: ", get_address(this), Logger::end_l());
}

void Texture::load_image(AssetManager::TextureInfo& textureInfo, const VkFormat format) {
	Logger::log_debug(Logger::tab(), "path: ", _path);
	Logger::log_debug(Logger::tab(), "width: ", _width, " and height: ", _height);
	Logger::log_debug(Logger::tab(), "mipmapping levels: ", _mipmap);

	// calculate the mipmap levels of the image
	// _mipmap = static_cast<uint32>(std::max(static_cast<int>(std::floor(std::log2(std::max(_width, _height)))) - 3, 1)); // since the last few are too small to be what I would consider useful, I'm subtracting it

	// create a staging buffer
	VulkanGPUBuffer stagingBuffer;
	VkDeviceSize imageMemSize = static_cast<uint64>(_width * _height * 4); /// @todo I don't know, the tutorial said 4 bytes per pixel, but T'm not to sure about it. Probably will make it a bit more dynamic
	stagingBuffer.create(Application::context()->device(), imageMemSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	// copy the image data into the staging buffer
	stagingBuffer.copy_data(textureInfo.data);

	// create the image and allocate its memory
	lassert(vmaCreateImage(
		Application::context()->device()->allocator(), 
		&get_image_create_info(
			format, 
			{ static_cast<uint32>(_width), static_cast<uint32>(_height), 1 },
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			_mipmap,
			static_cast<VkImageType>(textureInfo.dimension)
		),
		&get_alloc_create_info(Application::context()->device(), VMA_MEMORY_USAGE_GPU_ONLY),
		& _image, 
		& _memory, 
		nullptr
	) == VK_SUCCESS, "Failed to load image from path: ", _path);

	// convert the image layout and copy it from the buffer
	transition_layout(Application::context()->device(), Application::context()->commandBuffers(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_FORMAT_R8G8B8A8_SRGB, {VK_IMAGE_ASPECT_COLOR_BIT, 0, _mipmap, 0, 1});
	copy_from_buffer(&stagingBuffer, { static_cast<uint32>(_width), static_cast<uint32>(_height), 1 });
	// generate the mipmaps
	generate_mipmaps();

	// create the image view
	create_view(format, { VK_IMAGE_ASPECT_COLOR_BIT, 0, _mipmap, 0, 1 }, static_cast<VkImageViewType>(textureInfo.dimension));
}

void Texture::create_sampler(AssetManager::TextureInfo& textureInfo, const VkFilter magnifiedTexel, const VkFilter minimizedTexel, const VkSamplerMipmapMode mipmapMode) {
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(Application::context()->device()->physicalDevice(), &properties);

	VkSamplerCreateInfo samplerInfo {
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		nullptr,
		0,
		magnifiedTexel,
		minimizedTexel,
		mipmapMode,
		static_cast<VkSamplerAddressMode>(textureInfo.wrap),
		static_cast<VkSamplerAddressMode>(textureInfo.wrap),
		static_cast<VkSamplerAddressMode>(textureInfo.wrap),
		0.0f,
		static_cast<uint32>(textureInfo.anistropy),
		properties.limits.maxSamplerAnisotropy * Settings::Rendering::anistropy,
		VK_FALSE,
		VK_COMPARE_OP_NEVER,
		0.0f,
		static_cast<float>(_mipmap),
		static_cast<VkBorderColor>(textureInfo.alpha),
		VK_FALSE
	};

	lassert(vkCreateSampler(Application::context()->device()->device(), &samplerInfo, nullptr, &_sampler) == VK_SUCCESS, "Failed to create Vulkan image sampler!");

	Logger::log_debug(Logger::tab(), "Created image sampler at: ", get_address(this));
}

void Texture::generate_mipmaps() const {
	// check if image supports linear filtering
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(Application::context()->device()->physicalDevice(), VK_FORMAT_R8G8B8A8_SRGB, &formatProperties);
	lassert((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT), "Image does not support linear filtering with its current format!", Logger::end_l());

	// temporary command buffer for generating midmaps
	CommandBuffer cmdBuff = Application::context()->commandBuffers()->get_unused();
	// begin recording
	Application::context()->commandBuffers()->begin(cmdBuff, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	int32 mipWidth = _width, mipHeight = _height;

	for (uint32 i = 1; i < _mipmap; i++) {
		Application::context()->commandBuffers()->pipeline_barrier(
			cmdBuff,
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

		vkCmdBlitImage(Application::context()->commandBuffers()->commandBuffer(cmdBuff)->commandBuffer, _image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
			_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

		Application::context()->commandBuffers()->pipeline_barrier(
			cmdBuff,
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

	Application::context()->commandBuffers()->pipeline_barrier(
		cmdBuff,
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
	Application::context()->commandBuffers()->end(cmdBuff);

	// submit queues after recording
	Application::context()->commandBuffers()->submit_queue(cmdBuff, Application::context()->device()->graphicsQueue().queue);
	Application::context()->commandBuffers()->wait_queue(cmdBuff, Application::context()->device()->graphicsQueue().queue);
	// reset command buffer
	Application::context()->commandBuffers()->reset(cmdBuff);

	Logger::log_debug(Logger::tab(), "Created image mipmaps!");
}

void Texture::copy_from_buffer(const VulkanGPUBuffer* stagingBuffer, const VkExtent3D extent) {
	// temporary command buffer for copying
	CommandBuffer cmdBuff = Application::context()->commandBuffers()->get_unused();
	// begin recording
	Application::context()->commandBuffers()->begin(cmdBuff, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// copy image in buffer to the image
	VkBufferImageCopy imageCopy{
		0,
		0,
		0,
		{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
		{0, 0, 0},
		extent
	};

	vkCmdCopyBufferToImage(Application::context()->commandBuffers()->commandBuffer(cmdBuff)->commandBuffer, stagingBuffer->buffer(), _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

	// end recording
	Application::context()->commandBuffers()->end(cmdBuff);

	// submit queues after recording
	Application::context()->commandBuffers()->submit_queue(cmdBuff, Application::context()->device()->graphicsQueue().queue);
	Application::context()->commandBuffers()->wait_queue(cmdBuff, Application::context()->device()->graphicsQueue().queue);

	Logger::log_debug(Logger::tab(), "Copied image data from buffer to image at: ", get_address(this));
}

} // namespace lyra
