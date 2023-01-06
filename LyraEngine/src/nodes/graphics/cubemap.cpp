#include <nodes/graphics/cubemap.h>

#include <core/settings.h>

#include <rendering/assets.h>

#include <rendering/vulkan/vulkan_shader.h>
#include <rendering/vulkan/vulkan_pipeline.h>
#include <rendering/vulkan/GPU_buffer.h>

#include <nodes/graphics/camera.h>

namespace lyra {

CubemapBase::CubemapBase(
	const Array<const char*, 6>& paths,
	const char* vertexShaderPath,
	const char* fragShaderPath,
	Camera* const camera,
	const VkFormat& format,
	const ColorBlending& colorBlending,
	const Tessellation& tessellation,
	const Multisampling& multisampling
) : 
	GraphicsPipeline(
		camera, 
		{
			{ vulkan::Shader::Type::TYPE_VERTEX, vertexShaderPath, "main" },
			{ vulkan::Shader::Type::TYPE_FRAGMENT, fragShaderPath, "main" }
		}, 
		{
			{ 0, vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER, settings().rendering.maxFramesInFlight, vulkan::Shader::Type::TYPE_VERTEX },
			{ 0, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, settings().rendering.maxFramesInFlight, vulkan::Shader::Type::TYPE_FRAGMENT }
		}, 
		{},
		colorBlending,
		tessellation,
		multisampling
		),
	m_cubeMesh(
		{
			Mesh::Vertex({-1.0f,	-1.0f,	1.0f}, glm::vec3(), glm::vec3()),
			Mesh::Vertex({1.0f, 	-1.0f,  1.0f}, glm::vec3(), glm::vec3()),
			Mesh::Vertex({1.0f, 	-1.0f, 	-1.0f}, glm::vec3(), glm::vec3()),
			Mesh::Vertex({-1.0f, 	-1.0f, 	-1.0f}, glm::vec3(), glm::vec3()),
			Mesh::Vertex({-1.0f,	1.0f,  	1.0f}, glm::vec3(), glm::vec3()),
			Mesh::Vertex({1.0f,  	1.0f,  	1.0f}, glm::vec3(), glm::vec3()),
			Mesh::Vertex{{1.0f,  	1.0f, 	-1.0f}, glm::vec3(), glm::vec3()},
			Mesh::Vertex({-1.0f, 	1.0f, 	-1.0f}, glm::vec3(), glm::vec3())
		},
		{
			1, 2, 6,
			6, 5, 1,
			0, 4, 7,
			7, 3, 0,
			4, 5, 6,
			6, 7, 4,
			0, 3, 2,
			2, 1, 0,
			0, 1, 5,
			5, 4, 0,
			3, 7, 6,
			6, 2, 3
		},
		nullptr
	),
	m_cubeMeshRenderer(
		&m_cubeMesh, nullptr
	)
{
	{ // stuff for creating images
		// load all the images raw first
		Array<util::ImageData, 6> imageData;
		for (uint32 i = 0; i < 6; i++) imageData[i] = Assets::unpack_texture(paths[i]);

		// get the size of one of the images for future use
		auto width = imageData[0].width, height = imageData[0].height;
		// extent (size) of the image
		VkExtent3D imageExtent = {width, height, 1};
		
		const void* combinedImageData[6] = {
			imageData[0].data,
			imageData[1].data,
			imageData[2].data,
			imageData[3].data,
			imageData[4].data,
			imageData[5].data
		};

		// create the staging buffer
		vulkan::GPUBuffer stagingBuffer(width * height * 4 * 6, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		// copy the image data into the staging buffer
		stagingBuffer.copy_data(combinedImageData, 6, width * height * 4);

		// create the image
		vassert(Application::renderSystem.device.createImage(
			get_image_create_info(
				format,
				imageExtent,
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				1, 
				VK_IMAGE_TYPE_2D,
				6,
				VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
			),
			get_alloc_create_info(VMA_MEMORY_USAGE_GPU_ONLY),
			m_image, 
			m_memory), "create cubemap texture(s)");

		// transition the layout of the image
		transition_layout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_FORMAT_R8G8B8A8_SRGB, {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 6});
		// copy the buffer contents into the image
		copy_from_buffer(&stagingBuffer, imageExtent);
		
		// finally, create the image view
		create_view(format, { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 6 }, VK_IMAGE_VIEW_TYPE_CUBE);
	}

	{ // create a sampler for the image
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(Application::renderSystem.device.physicalDevice(), &properties);

		VkSamplerCreateInfo samplerInfo {
			VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			nullptr,
			0,
			VK_FILTER_LINEAR,
			VK_FILTER_LINEAR,
			VK_SAMPLER_MIPMAP_MODE_LINEAR,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			0.0f,
			VK_FALSE,
			1.0f, // properties.limits.maxSamplerAnisotropy * settings().rendering.anistropy,
			VK_FALSE,
			VK_COMPARE_OP_ALWAYS,
			0.0f,
			0.0f,
			VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
			VK_FALSE
		};

		vassert(vkCreateSampler(Application::renderSystem.device.device(), &samplerInfo, nullptr, &m_sampler), "create Vulkan image sampler!");
	}

	{ // next, create the descriptors
		// write the bindings
		vulkan::Descriptor::Writer writer;
		writer.add_writes({
			{ get_descriptor_cubemap_info(), 1, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER}
		});
		writer.add_writes({
			{ camera->m_buffers.at(0).get_descriptor_buffer_info(), 0, lyra::vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER },
			{ camera->m_buffers.at(1).get_descriptor_buffer_info(), 0, lyra::vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER }
		});

		// create both descriptors
		for (uint32 i = 0; i < settings().rendering.maxFramesInFlight; i++) 
			m_descriptors.emplace_back(
				m_descriptorSetLayout.get(),
				0,
				m_descriptorPool.get(), 
				writer
			);
	}
}

void CubemapBase::draw() const {
	Application::renderSystem.currentCommandBuffer.bindPipeline(bindPoint(), pipeline());
	Application::renderSystem.currentCommandBuffer.bindDescriptorSet(
		bindPoint(), 
		layout(),
		0, 
		m_descriptors[Application::renderSystem.currentFrame()].get());
	m_cubeMeshRenderer.draw();
}

} // namespace lyra
