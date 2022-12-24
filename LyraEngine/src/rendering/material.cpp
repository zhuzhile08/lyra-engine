#include <rendering/material.h>

#include <rendering/vulkan/descriptor.h>
#include <rendering/graphics_pipeline.h>
#include <rendering/texture.h>
#include <nodes/graphics/camera.h>
#include <nodes/mesh/mesh_renderer.h>

#include <core/application.h>

namespace lyra {

Material::Material(
	Camera* const camera,
	const std::vector<MeshRenderer*>& meshRenderers,
	const Color& albedoColor,
	const Texture* const albedoTexture,
	const uint8& metallic,
	const uint8& roughness,
	const uint8& specular,
	const Texture* const metallicTexture,
	const bool& emissionEnabled,
	const Color& emissionColor,
	const Texture* const emissionTexture,
	const uint8& emissionEnergy,
	const Texture* const normalMapTexture,
	const int8& normalMapValue,
	const Texture* const displacementMapTexture,
	const uint8& displacementMapValue,
	const Texture* const  occlusionMapTexture,
	const uint8& occlusionMapValue
) : m_meshRenderers(meshRenderers),
	albedoColor(albedoColor),
	albedoTexture(albedoTexture),
	metallic(metallic),
	roughness(roughness),
	specular(specular),
	metallicTexture(metallicTexture),
	emissionEnabled(emissionEnabled),
	emissionColor(emissionColor),
	emissionTexture(emissionTexture),
	emissionEnergy(emissionEnergy),
	normalMapTexture(normalMapTexture),
	normalMapValue(normalMapValue),
	displacementMapTexture(displacementMapTexture),
	displacementMapValue(displacementMapValue),
	occlusionMapTexture(occlusionMapTexture),
	occlusionMapValue(occlusionMapValue),
	camera(camera)
{
	// preallocate the memory for the vertex and fragment shaders
	m_vertShaderBuffers.reserve(Settings::Rendering::maxFramesInFlight);
	m_fragShaderBuffers.reserve(Settings::Rendering::maxFramesInFlight);

	// uniform data to send to the vertex shader
	MaterialVertexData vertDat{
		displacementMapValue, normalMapValue
	};

	// uniform data to send to the fragment shader
	MaterialFragmentData fragDat{
		albedoColor.vec(),
		emissionColor.vec(),
		metallic,
		roughness,
		specular,
		emissionEnabled,
		emissionEnergy,
		occlusionMapValue
	};

	for (uint32 i = 0; i < Settings::Rendering::maxFramesInFlight; i++) { 
		// create the buffers that send information to the vertex shader and copy in the information
		m_vertShaderBuffers.emplace_back(sizeof(MaterialVertexData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		m_vertShaderBuffers.at(i).copy_data(&vertDat);
		// create the buffers that send information to the fragment shader and copy in the information
		m_fragShaderBuffers.emplace_back(sizeof(MaterialFragmentData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		m_fragShaderBuffers.at(i).copy_data(&fragDat);
	}

	// create the descriptor set
	vulkan::Descriptor::Writer writer;
	writer.add_writes({ // write the images
		{ (normalMapTexture) ? normalMapTexture->get_descriptor_image_info() : Assets::nullNormal()->get_descriptor_image_info(), 3, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER},
		{ (displacementMapTexture) ? displacementMapTexture->get_descriptor_image_info() : Assets::nullTexture()->get_descriptor_image_info(), 4, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER},
		{ (albedoTexture) ? albedoTexture->get_descriptor_image_info() : Assets::nullTexture()->get_descriptor_image_info(), 2, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER},
		{ (metallicTexture) ? metallicTexture->get_descriptor_image_info() : Assets::nullTexture()->get_descriptor_image_info(), 6, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER},
		{ (emissionTexture) ? emissionTexture->get_descriptor_image_info() : Assets::nullTexture()->get_descriptor_image_info(), 7, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER},
		{ (occlusionMapTexture) ? occlusionMapTexture->get_descriptor_image_info() : Assets::nullTexture()->get_descriptor_image_info(), 8, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER},
		});
	writer.add_writes({ // write the buffers
		{ m_vertShaderBuffers[0].get_descriptor_buffer_info(), 1, lyra::vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER },
		{ m_vertShaderBuffers[1].get_descriptor_buffer_info(), 1, lyra::vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER },
		{ m_fragShaderBuffers[0].get_descriptor_buffer_info(), 5, lyra::vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER },
		{ m_fragShaderBuffers[1].get_descriptor_buffer_info(), 5, lyra::vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER }
		});

	// create the descriptors
	m_descriptors.reserve(Settings::Rendering::maxFramesInFlight);
	for (uint32 i = 0; i < Settings::Rendering::maxFramesInFlight; i++) 
		m_descriptors.emplace_back(
			camera->m_renderPipeline->descriptorSetLayout(),
			1,
			camera->m_renderPipeline->descriptorPool(), 
			writer
		);

	this->camera->m_materials.push_back(this);
}

Material& Material::operator=(const Material& other) {

	return *this;
}

Material& Material::operator=(Material&& other) {

	return *this;
}

void Material::draw() const {
	// bind the descriptor set first
	Application::renderSystem.currentCommandBuffer.bindDescriptorSet(
		camera->m_renderPipeline->bindPoint(), 
		camera->m_renderPipeline->layout(), 
		1, 
		m_descriptors.at(Application::renderSystem.currentFrame()).get()
	);

	for (uint32 i = 0; i < m_meshRenderers.size(); i++) m_meshRenderers.at(i)->draw();
}

} // namespace lyra
