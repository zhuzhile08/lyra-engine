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
	// uniform data to send to the vertex shader
	MaterialVertexData vertDat {
		displacementMapValue, normalMapValue
	};

	// uniform data to send to the fragment shader
	MaterialFragmentData fragDat {
		albedoColor.vec(),
		emissionColor.vec(),
		metallic,
		roughness,
		specular,
		emissionEnabled,
		emissionEnergy,
		occlusionMapValue
	};

	for (uint32 i = 0; i < Settings::RenderConfig::maxFramesInFlight; i++) { 
		// create the buffers that send information to the vertex shader and copy in the information
		m_vertShaderBuffers[i] = m_vertShaderBuffers[i].create(sizeof(MaterialVertexData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		m_vertShaderBuffers[i]->copy_data(&vertDat);
		// create the buffers that send information to the fragment shader and copy in the information
		m_fragShaderBuffers[i] =  m_vertShaderBuffers[i].create(sizeof(MaterialFragmentData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		m_fragShaderBuffers[i]->copy_data(&fragDat);
	}

	// create the descriptors themselves
	// for (auto& descriptorSet : m_descriptorSets) {
	for (uint32 i = 0; i < Settings::RenderConfig::maxFramesInFlight; i++) {
		// get a unused descriptor set and push back its pointer
		m_descriptorSets[i] = camera->m_renderPipeline->descriptorSystem(1).get_unused_set();
		// add the writes
		m_descriptorSets[i]->add_writes({ // write the images
			{ (normalMapTexture) ? normalMapTexture->get_descriptor_image_info() : Assets::nullNormal()->get_descriptor_image_info(), 3, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER},
			{ (displacementMapTexture) ? displacementMapTexture->get_descriptor_image_info() : Assets::nullTexture()->get_descriptor_image_info(), 4, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER},
			{ (albedoTexture) ? albedoTexture->get_descriptor_image_info() : Assets::nullTexture()->get_descriptor_image_info(), 2, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER},
			{ (metallicTexture) ? metallicTexture->get_descriptor_image_info() : Assets::nullTexture()->get_descriptor_image_info(), 6, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER},
			{ (emissionTexture) ? emissionTexture->get_descriptor_image_info() : Assets::nullTexture()->get_descriptor_image_info(), 7, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER},
			{ (occlusionMapTexture) ? occlusionMapTexture->get_descriptor_image_info() : Assets::nullTexture()->get_descriptor_image_info(), 8, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER},
		});
		m_descriptorSets[i]->add_writes({ // write the buffers
			{ m_vertShaderBuffers[0]->get_descriptor_buffer_info(), 1, lyra::vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_UNIFORM_BUFFER },
			{ m_vertShaderBuffers[1]->get_descriptor_buffer_info(), 1, lyra::vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_UNIFORM_BUFFER },
			{ m_fragShaderBuffers[0]->get_descriptor_buffer_info(), 5, lyra::vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_UNIFORM_BUFFER },
			{ m_fragShaderBuffers[1]->get_descriptor_buffer_info(), 5, lyra::vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_UNIFORM_BUFFER }
		});
		// update the descriptor set
		m_descriptorSets[i]->update();
	}

	this->camera->m_materials.push_back(this);
}

// Material& Material::operator=(const Material& other) {

// 	return *this;
// }

// Material& Material::operator=(Material&& other) {

// 	return *this;
// }

void Material::draw() const {
	// bind the descriptor set first
	Application::renderSystem.frames[Application::renderSystem.currentFrame()].commandBuffer().bindDescriptorSet(
		camera->m_renderPipeline->bindPoint(), 
		camera->m_renderPipeline->layout(), 
		1, 
		*m_descriptorSets[Application::renderSystem.currentFrame()]
	);

	for (uint32 i = 0; i < m_meshRenderers.size(); i++) m_meshRenderers.at(i)->draw();
}

} // namespace lyra
