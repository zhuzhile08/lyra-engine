#include <core/rendering/material.h>

#include <core/rendering/vulkan/descriptor.h>
#include <core/rendering/graphics_pipeline.h>
#include <core/rendering/texture.h>
#include <nodes/mesh/mesh_renderer.h>

#include <core/application.h>

namespace lyra {

Material::Material(
	const Camera* const camera,
	const std::vector<MeshRenderer*> meshRenderers,
	const Color albedoColor,
	const Texture* const albedoTexture,
	const uint8 metallic,
	const uint8 roughness,
	const uint8 specular,
	const Texture* const metallicTexture,
	const bool emissionEnabled,
	const Color emissionColor,
	const Texture* const emissionTexture,
	const uint8 emissionEnergy,
	const Texture* const normalMapTexture,
	const int8 normalMapValue,
	const Texture* const heightMapTexture,
	const uint8 heightMapValue,
	const Texture* const  occlusionMapTexture,
	const uint8 occlusionMapValue
) : m_meshRenderers(meshRenderers),
	m_albedoColor(albedoColor),
	m_albedoTexture(albedoTexture),
	m_metallic(metallic),
	m_roughness(roughness),
	m_specular(specular),
	m_metallicTexture(metallicTexture),
	m_emissionEnabled(emissionEnabled),
	m_emissionColor(emissionColor),
	m_emissionTexture(emissionTexture),
	m_emissionEnergy(emissionEnergy),
	m_normalMapTexture(normalMapTexture),
	m_normalMapValue(normalMapValue),
	m_heightMapTexture(heightMapTexture),
	m_heightMapValue(heightMapValue),
	m_occlusionMapTexture(occlusionMapTexture),
	m_occlusionMapValue(occlusionMapValue),
	camera(camera)
{
	Logger::log_info("Creating material...");

	// preallocate the memory for the vertex and fragment shaders
	m_vertShaderBuffers.reserve(Settings::Rendering::maxFramesInFlight);
	m_fragShaderBuffers.reserve(Settings::Rendering::maxFramesInFlight);

	for (uint32 i = 0; i < Settings::Rendering::maxFramesInFlight; i++) { 
		// create the buffers that send information to the vertex shader
		m_vertShaderBuffers.emplace_back(sizeof(MaterialVertexData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		// create the buffers that send information to the fragment shader
		m_fragShaderBuffers.emplace_back(sizeof(MaterialFragmentData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	}

	// create the descriptor set
	vulkan::Descriptor::Writer writer;
	writer.add_writes({ // write the camera buffers into the descriptor first since they are always present
		{ nullptr, &camera->m_buffers.at(0).get_descriptor_buffer_info(), 0, lyra::vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER },
		{ nullptr, &camera->m_buffers.at(1).get_descriptor_buffer_info(), 0, lyra::vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER },
		{ nullptr, &m_vertShaderBuffers.at(0).get_descriptor_buffer_info(), 1, lyra::vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER },
		{ nullptr, &m_vertShaderBuffers.at(1).get_descriptor_buffer_info(), 1, lyra::vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER },
		{ (m_normalMapTexture) ? &m_normalMapTexture->get_descriptor_image_info() : nullptr, nullptr, 2, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER},
		{ (m_heightMapTexture) ? &m_heightMapTexture->get_descriptor_image_info() : nullptr, nullptr, 3, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER},
		{ nullptr, &m_fragShaderBuffers.at(0).get_descriptor_buffer_info(), 4, lyra::vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER },
		{ nullptr, &m_fragShaderBuffers.at(1).get_descriptor_buffer_info(), 4, lyra::vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER },
		{ (m_albedoTexture) ? &m_albedoTexture->get_descriptor_image_info() : nullptr, nullptr, 5, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER},
		{ (m_metallicTexture) ? &m_metallicTexture->get_descriptor_image_info() : nullptr, nullptr, 6, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER},
		{ (m_emissionTexture) ? &m_emissionTexture->get_descriptor_image_info() : nullptr, nullptr, 7, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER},
		{ (m_occlusionMapTexture) ? &m_occlusionMapTexture->get_descriptor_image_info() : nullptr, nullptr, 8, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER},
		});

	// create the descriptors
	m_descriptors.reserve(Settings::Rendering::maxFramesInFlight);
	for (uint32 i = 0; i < Settings::Rendering::maxFramesInFlight; i++) m_descriptors.emplace_back(camera->m_renderPipeline->descriptorSetLayout(), camera->m_renderPipeline->descriptorPool(), writer);

	Logger::log_info("Successfully created material with address:", get_address(this), "!");
}

void Material::draw() const {
	// bind the descriptor set first
	vkCmdBindDescriptorSets(
		Application::renderSystem()->activeCommandBuffer(),
		camera->m_renderPipeline->bindPoint(), camera->m_renderPipeline->layout(), 0, 1, m_descriptors.at(Application::renderSystem()->currentFrame()).get_ptr(), 0, nullptr
	);
	// then draw all the meshes
	for (int i = 0; i < m_meshRenderers.size(); i++) m_meshRenderers.at(i)->draw();
}

} // namespace lyra
