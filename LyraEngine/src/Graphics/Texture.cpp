#include <Resource/Texture.h>

#include <Common/Config.h>

namespace lyra {

Texture::Texture(const util::detail::LoadedTexture& imageData, const VkFormat& format)
	: m_width(imageData.width), m_height(imageData.height), m_mipmap(imageData.mipmap) {
	{
		// calculate the mipmap levels of the image
		m_mipmap = static_cast<uint32>(std::max(static_cast<int>(std::floor(std::log2(std::max(m_width, m_height)))) - 3, 1)); // since the last few are too small to be what I would consider useful, I'm subtracting it

		// create a staging buffer
		vulkan::GPUBuffer stagingBuffer(m_width * m_height * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		// copy the image data into the staging buffer
		stagingBuffer.copyData(imageData.data);

		// extent (size) of the image
		VkExtent3D imageExtent = { m_width, m_height, 1 };

		// create the image and allocate its memory
		m_image = vulkan::vk::Image(
			Application::renderSystem.device.device(),
			Application::renderSystem.device.allocator(),
			getImageCreateInfo(
				format, 
				imageExtent,
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				m_mipmap,
				static_cast<VkImageType>(imageData.dimension)
			),
			getAllocCreateInfo(VMA_MEMORY_USAGE_GPU_ONLY),
			m_memory
		);

		// convert the image layout and copy it from the buffer
		transitionLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_FORMAT_R8G8B8A8_SRGB, {VK_IMAGE_ASPECT_COLOR_BIT, 0, m_mipmap, 0, 1});
		copyFromBuffer(&stagingBuffer, imageExtent);
		// generate the mipmaps
		if (m_mipmap != 0) generate_mipmaps();
	}

	// create the image view
	createView(format, { VK_IMAGE_ASPECT_COLOR_BIT, 0, m_mipmap, 0, 1 }, static_cast<VkImageViewType>(imageData.dimension));
	// finally create the image sampler
	createSampler(imageData);
}

void Texture::createSampler(const util::detail::LoadedTexture& imageData, const VkFilter& magnifiedTexel, const VkFilter& minimizedTexel, const VkSamplerMipmapMode& mipmapMode) {
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(Application::renderSystem.device.physicalDevice(), &properties);

	VkSamplerCreateInfo samplerInfo {
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		nullptr,
		0,
		magnifiedTexel,
		minimizedTexel,
		mipmapMode,
		static_cast<VkSamplerAddressMode>(imageData.wrap),
		static_cast<VkSamplerAddressMode>(imageData.wrap),
		static_cast<VkSamplerAddressMode>(imageData.wrap),
		0.0f,
		static_cast<uint32>(imageData.anistropy),
		properties.limits.maxSamplerAnisotropy * config::anistropy,
		VK_FALSE,
		VK_COMPARE_OP_ALWAYS,
		0.0f,
		static_cast<float32>(m_mipmap),
		static_cast<VkBorderColor>(imageData.alpha),
		VK_FALSE
	};

	m_sampler = vulkan::vk::Sampler(Application::renderSystem.device.device(), samplerInfo);
}

void Texture::generate_mipmaps() const {
	// check if image supports linear filtering
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(Application::renderSystem.device.physicalDevice(), VK_FORMAT_R8G8B8A8_SRGB, &formatProperties);
	ASSERT((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT), "Image does not support linear filtering with its current format!\n");

	// temporary command buffer for generating midmaps
	vulkan::CommandBuffer cmdBuff(vulkan::CommandBuffer::Usage::USAGE_ONE_TIME_SUBMIT);
	// begin recording
	cmdBuff.begin();

	int32 mipWidth = m_width, mipHeight = m_height;

	for (uint32 i = 1; i < m_mipmap; i++) {
		cmdBuff.pipelineBarrier(
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT, 
			VkMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM },
			VkBufferMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM },
			getImageMemoryBarrier(
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

		cmdBuff.blitImage(m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, blit, VK_FILTER_LINEAR);

		cmdBuff.pipelineBarrier(
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VkMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM },
			VkBufferMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM },
			getImageMemoryBarrier(
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

	cmdBuff.pipelineBarrier(
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		VkMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM },
		VkBufferMemoryBarrier{ VK_STRUCTURE_TYPE_MAX_ENUM },
		getImageMemoryBarrier(
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_SHADER_READ_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			{ VK_IMAGE_ASPECT_COLOR_BIT, m_mipmap - 1, 1, 0, 1 }
		)
	);

	// end recording
	cmdBuff.end();

	// submit queues after recording
	cmdBuff.submitQueue(Application::renderSystem.device.graphicsQueue());
	// reset command buffer
	cmdBuff.reset();
}

} // namespace lyra
