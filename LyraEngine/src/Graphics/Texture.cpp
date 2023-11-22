#include <Graphics/Texture.h>

#include <algorithm>

namespace lyra {

Texture::Texture(const resource::TextureFile& imageData, const VkFormat& format) : 
	m_type(static_cast<Type>(imageData.type)),
	m_width(imageData.width), 
	m_height(imageData.height)
{
	{
		// create a staging buffer
		vulkan::GPUBuffer stagingBuffer(m_width * m_height * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		// copy the image data into the staging buffer
		stagingBuffer.copyData(imageData.data.data());

		// extent (size) of the image
		VkExtent3D imageExtent = { m_width, m_height, 1 };

		// create the image and allocate its memory
		m_image.createImage(
			m_image.imageCreateInfo(
				format, 
				imageExtent,
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				imageData.mipmap,
				static_cast<VkImageType>(imageData.dimension)
			),
			m_memory.getAllocCreateInfo(VMA_MEMORY_USAGE_GPU_ONLY),
			m_memory.memory
		);

		// convert the image layout and copy it from the buffer
		m_image.transitionLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, {VK_IMAGE_ASPECT_COLOR_BIT, 0, imageData.mipmap, 0, 1});
		m_image.copyFromBuffer(stagingBuffer, imageExtent);

		// generate the mipmaps
		if (imageData.mipmap != 0) {
			// check if image supports linear filtering
			ASSERT(m_image.formatProperties().optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT, "Image does not support linear filtering with its current format!\n");

			// temporary command buffer for generating midmaps
			vulkan::CommandQueue cmdQueue;
			cmdQueue.oneTimeBegin();

			int32 mipWidth = m_width, mipHeight = m_height;

			for (uint32 i = 1; i < imageData.mipmap; i++) {
				cmdQueue.activeCommandBuffer->pipelineBarrier(
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT, 
					0,
					{ },
					{ },
					m_image.imageMemoryBarrier(
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

				cmdQueue.activeCommandBuffer->blitImage(
					m_image.image, 
					VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
					m_image.image, 
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
					blit, 
					VK_FILTER_LINEAR
				);

				cmdQueue.activeCommandBuffer->pipelineBarrier(
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					0,
					{ },
					{ },
					m_image.imageMemoryBarrier(
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

			cmdQueue.activeCommandBuffer->pipelineBarrier(
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				0,
				{ },
				{ },
				m_image.imageMemoryBarrier(
					VK_ACCESS_TRANSFER_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					{ VK_IMAGE_ASPECT_COLOR_BIT, imageData.mipmap - 1, 1, 0, 1 }
				)
			);

			cmdQueue.oneTimeSubmit();
		}
	}

	// create the image view
	m_image.createView(format, { VK_IMAGE_ASPECT_COLOR_BIT, 0, imageData.mipmap, 0, 1 }, static_cast<VkImageViewType>(imageData.dimension));

	m_sampler = m_image.createSampler(
		static_cast<VkSamplerAddressMode>(imageData.wrap),
		static_cast<VkSamplerAddressMode>(imageData.wrap),
		static_cast<VkSamplerAddressMode>(imageData.wrap),
		static_cast<VkBorderColor>(imageData.alpha),
		static_cast<float32>(imageData.mipmap)
	);
}

} // namespace lyra
