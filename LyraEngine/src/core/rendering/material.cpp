#include <core/rendering/material.h>

#include <core/rendering/vulkan/descriptor.h>
#include <core/rendering/graphics_pipeline.h>
#include <nodes/mesh/mesh_renderer.h>

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
	const Texture* const heightMap,
	const uint8 heightMapValue,
	const Texture* const  occlusionMap,
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
	_heightMap(heightMap),
	_heightMapValue(heightMapValue),
	_occlusionMap(occlusionMap),
	_occlusionMapValue(occlusionMapValue),
	camera(camera)
{
	Logger::log_info("Creating material...");

	// create the descriptor set
	VulkanDescriptor::Writer writer;
	writer.add_writes({ {

	} });

	_descriptor = std::make_shared<VulkanDescriptor>(camera->_renderPipeline->descriptorSetLayout(), camera->_renderPipeline->descriptorPool(), writer);

	Logger::log_info("Successfully created material with address:", get_address(this), "!");
}

void Material::draw() const {
	// bind the descriptor set first
	vkCmdBindDescriptorSets(
		Context::get()->renderSystem()->activeCommandBuffer(),
		camera->_renderPipeline->bindPoint(), camera->_renderPipeline->layout(), 0, 1, _descriptor->get_ptr(), 0, nullptr
	);
	// then draw all the meshes
	for (int i = 0; i < _meshRenderers.size(); i++) _meshRenderers.at(i)->draw();
}

} // namespace lyra
