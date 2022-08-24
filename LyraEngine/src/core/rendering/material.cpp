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
) : _meshRenderers(meshRenderers),
	_albedoColor(albedoColor),
	_albedoTexture(albedoTexture),
	_metallic(metallic),
	_roughness(roughness),
	_specular(specular),
	_metallicTexture(metallicTexture),
	_emissionEnabled(emissionEnabled),
	_emissionColor(emissionColor),
	_emissionTexture(emissionTexture),
	_emissionEnergy(emissionEnergy),
	_normalMapTexture(normalMapTexture),
	_normalMapValue(normalMapValue),
	_heightMapTexture(heightMapTexture),
	_heightMapValue(heightMapValue),
	_occlusionMapTexture(occlusionMapTexture),
	_occlusionMapValue(occlusionMapValue),
	camera(camera)
{
	Logger::log_info("Creating material...");

	// create the buffers that send information to the vertex shader
	_vertShaderBuffers.reserve(Settings::Rendering::maxFramesInFlight);
	for (uint32 i = 0; i < Settings::Rendering::maxFramesInFlight; i++) _vertShaderBuffers.emplace_back(sizeof(MaterialVertexData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	// create the buffers that send information to the fragment shader
	_vertShaderBuffers.reserve(Settings::Rendering::maxFramesInFlight);
	for (uint32 i = 0; i < Settings::Rendering::maxFramesInFlight; i++) _vertShaderBuffers.emplace_back(sizeof(MaterialFragmentData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	// create the descriptor set
	VulkanDescriptor::Writer writer;
	writer.add_writes({ // write the camera buffers into the descriptor first since they are always present
		{ nullptr, &camera->_buffers.at(0).get_descriptor_buffer_info(), 0, lyra::VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER },
		{ nullptr, &camera->_buffers.at(1).get_descriptor_buffer_info(), 0, lyra::VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER },
		{ nullptr, &_vertShaderBuffers.at(0).get_descriptor_buffer_info(), 1, lyra::VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER},
		{ nullptr, &_vertShaderBuffers.at(1).get_descriptor_buffer_info(), 1, lyra::VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER}
		// { nullptr, &_fragShaderBuffers.at(0).get_descriptor_buffer_info(), 5, lyra::VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER},
		// { nullptr, &_fragShaderBuffers.at(1).get_descriptor_buffer_info(), 5, lyra::VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER}
		});

	// check if the textures are present and add them to the descriptor if so
	if (_normalMapTexture) writer.add_writes({ { &_normalMapTexture->get_descriptor_image_info(), nullptr, 2, VulkanDescriptor::Type::TYPE_IMAGE_SAMPLER } });
	if (_heightMapTexture) writer.add_writes({ { &_heightMapTexture->get_descriptor_image_info(), nullptr, 3, VulkanDescriptor::Type::TYPE_IMAGE_SAMPLER } });
	if (_albedoTexture) writer.add_writes({ { &_albedoTexture->get_descriptor_image_info(), nullptr, 4, VulkanDescriptor::Type::TYPE_IMAGE_SAMPLER } });
	// if (_metallicTexture) writer.add_writes({ { &_metallicTexture->get_descriptor_image_info(), nullptr, 5, VulkanDescriptor::Type::TYPE_IMAGE_SAMPLER } });
	// if (_emissionTexture) writer.add_writes({ { &_emissionTexture->get_descriptor_image_info(), nullptr, 6, VulkanDescriptor::Type::TYPE_IMAGE_SAMPLER } });
	// if (_occlusionMapTexture) writer.add_writes({ { &_occlusionMapTexture->get_descriptor_image_info(), nullptr, 7, VulkanDescriptor::Type::TYPE_IMAGE_SAMPLER } });

	// create the descriptors
	_descriptors.reserve(Settings::Rendering::maxFramesInFlight);
	for (uint32 i = 0; i < Settings::Rendering::maxFramesInFlight; i++) _descriptors.emplace_back(camera->_renderPipeline->descriptorSetLayout(), camera->_renderPipeline->descriptorPool(), writer);

	Logger::log_info("Successfully created material with address:", get_address(this), "!");
}

void Material::draw() const {
	// bind the descriptor set first
	vkCmdBindDescriptorSets(
		Application::renderSystem()->activeCommandBuffer(),
		camera->_renderPipeline->bindPoint(), camera->_renderPipeline->layout(), 0, 1, _descriptors.at(Application::renderSystem()->currentFrame()).get_ptr(), 0, nullptr
	);
	// then draw all the meshes
	for (int i = 0; i < _meshRenderers.size(); i++) _meshRenderers.at(i)->draw();
}

} // namespace lyra
