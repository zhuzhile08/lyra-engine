#include <ECS/Cubemap.h>

#include <Common/Config.h>

#include <Resource/ResourceSystem.h>

#include <Resource/Shader.h>
#include <Graphics/VulkanImpl/PipelineBase.h>
#include <Graphics/VulkanImpl/GPUBuffer.h>

#include <ECS/Camera.h>

namespace lyra {

CubemapBase::CubemapBase(
	const Array<std::string_view, 6>& paths,
	std::string_view vertexShaderPath,
	std::string_view fragShaderPath,
	Camera* const camera,
	const Image::Format& format,
	const ColorBlending& colorBlending,
	const Tessellation& tessellation,
	const Multisampling& multisampling
) : 
	GraphicsPipeline(
		camera, 
		{
			{ vulkan::Shader::Type::VERTEX, vertexShaderPath, "main" },
			{ vulkan::Shader::Type::FRAGMENT, fragShaderPath, "main" }
		}, 
		{
			{ 0, vulkan::DescriptorSystem::DescriptorSets::Type::uniformBuffer, config::maxFramesInFlight, vulkan::Shader::Type::VERTEX },
			{ 0, vulkan::DescriptorSystem::DescriptorSets::Type::imageSampler, config::maxFramesInFlight, vulkan::Shader::Type::FRAGMENT }
		}, 
		{},
		colorBlending,
		tessellation,
		multisampling
		),
	m_cubeMesh(
		{
			Mesh::Vertex({-1.0f,	-1.0f,	1.0f}, glm::vec3(), glm::vec3()),
			Mesh::Vertex({1.0f, 	-1.0f, 1.0f}, glm::vec3(), glm::vec3()),
			Mesh::Vertex({1.0f, 	-1.0f, 	-1.0f}, glm::vec3(), glm::vec3()),
			Mesh::Vertex({-1.0f, 	-1.0f, 	-1.0f}, glm::vec3(), glm::vec3()),
			Mesh::Vertex({-1.0f,	1.0f, 	1.0f}, glm::vec3(), glm::vec3()),
			Mesh::Vertex({1.0f, 	1.0f, 	1.0f}, glm::vec3(), glm::vec3()),
			Mesh::Vertex{{1.0f, 	1.0f, 	-1.0f}, glm::vec3(), glm::vec3()},
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
		Array<resource::TextureFile, 6> imageData;
		for (uint32 i = 0; i < 6; i++) imageData[i] = ResourceSystem::unpack_texture(paths[i]);

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
		stagingBuffer.copyData(combinedImageData, 6, width * height * 4);

		// create the image
		VULKAN_ASSERT(Application::renderer.device.createImage(
			imageCreateInfo(
				format,
				imageExtent,
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				1, 
				VK_IMAGE_TYPE_2D,
				6,
				VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
			),
			getAllocCreateInfo(VMA_MEMORY_USAGE_GPU_ONLY),
			m_image, 
			m_memory), "create cubemap texture(s)");

		// transition the layout of the image
		transitionLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_FORMAT_R8G8B8A8_SRGB, {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 6});
		// copy the buffer contents into the image
		copyFromBuffer(&stagingBuffer, imageExtent);
		
		//ly, create the image view
		createView(format, { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 6 }, VK_IMAGE_VIEW_TYPE_CUBE);
	}

	{ // create a sampler for the image
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(Application::renderer.device.physicalDevice(), &properties);

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
			1.0f, // properties.limits.maxSamplerAnisotropy * config::anistropy,
			VK_FALSE,
			VK_COMPARE_OP_ALWAYS,
			0.0f,
			0.0f,
			VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
			VK_FALSE
		};

		m_sampler = vulkan::vk::Sampler(Application::renderer.device.device(), samplerInfo);
	}

	{ // next, create the descriptors
		// write the bindings
		vulkan::DescriptorSystem::DescriptorSets::Writer writer;
		writer.addWrites({
			{ getDescriptorcubemapInfo(), 1, vulkan::DescriptorSystem::DescriptorSets::Type::imageSampler}
		});
		writer.addWrites({
			{ camera->m_buffers.at(0).getDescriptorBufferInfo(), 0, lyra::vulkan::DescriptorSystem::DescriptorSets::Type::uniformBuffer },
			{ camera->m_buffers.at(1).getDescriptorBufferInfo(), 0, lyra::vulkan::DescriptorSystem::DescriptorSets::Type::uniformBuffer }
		});

		// create both descriptors
		for (uint32 i = 0; i < config::maxFramesInFlight; i++) 
			m_descriptorSets.emplaceBack(
				m_descriptorSetLayout,
				0,
				m_descriptorPool, 
				writer
			);
	}
}

void CubemapBase::draw() const {
	Application::renderer.frames[Application::renderer.currentFrame()].commandBuffer().bindPipeline(bindPoint(), pipeline());
	Application::renderer.frames[Application::renderer.currentFrame()].commandBuffer().bindDescriptorSet(
		bindPoint(), 
		layout(),
		0, 
		m_descriptorSets[Application::renderer.currentFrame()]);
	m_cubeMeshRenderer.draw();
}

} // namespace lyra
