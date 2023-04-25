#include <Resource/Material.h>

#include <Graphics/VulkanImpl/DescriptorSystem.h>
#include <Graphics/GraphicsPipeline.h>

#include <Resource/Texture.h>
#include <Resource/ResourceManager.h>

#include <EntitySystem/Camera.h>
#include <EntitySystem/MeshRenderer.h>

#include <Application/Application.h>

namespace lyra {

// material data
Material::Material(
	const Color& albedoColor,
	std::string_view albedoTexturePath,
	const float& metallic,
	const float& roughness,
	std::string_view metallicTexturePath,
	const Color& specularColor,
	std::string_view specularTexturePath,
	const Color& emissionColor,
	std::string_view emissionTexturePath,
	std::string_view normalMapTexturePath,
	std::string_view displacementMapTexturePath,
	const Color& occlusionColor,
	std::string_view occlusionMapTexturePath
) : albedoColor(albedoColor),
	albedoTexture(albedoTexturePath, (albedoTexturePath.empty() ? ResourceManager::texture(albedoTexturePath) : ResourceManager::nullTexture())),
	metallic(metallic),
	roughness(roughness),
	metallicTexture(metallicTexturePath, (metallicTexturePath.empty() ? ResourceManager::texture(metallicTexturePath) : ResourceManager::nullTexture())),
	specularColor(specularColor),
	specularTexture(specularTexturePath, (specularTexturePath.empty() ? ResourceManager::texture(specularTexturePath) : ResourceManager::nullTexture())),
	emissionColor(emissionColor),
	emissionTexture(emissionTexturePath, (emissionTexturePath.empty() ? ResourceManager::texture(emissionTexturePath) : ResourceManager::nullTexture())),
	normalMapTexture(normalMapTexturePath, (normalMapTexturePath.empty() ? ResourceManager::texture(normalMapTexturePath) : ResourceManager::nullNormal())),
	displacementMapTexture(displacementMapTexturePath, (displacementMapTexturePath.empty() ? ResourceManager::texture(displacementMapTexturePath) : ResourceManager::nullTexture())),
	occlusionColor(occlusionColor),
	occlusionMapTexture(occlusionMapTexturePath, (occlusionMapTexturePath.empty() ? ResourceManager::texture(occlusionMapTexturePath) : ResourceManager::nullTexture()))
{
	/* uniform data to send to the vertex shader
	MaterialVertexData vertDat {
		displacementMapValue, normalMapValue
	};

	/ uniform data to send to the fragment shader
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

	/ create the buffers that send information to the vertex shader and copy in the information
	for (auto& vertShaderBuffer : m_vertShaderBuffers) {
		vertShaderBuffer = vertShaderBuffer.create(sizeof(MaterialVertexData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		vertShaderBuffer->copy_data(&vertDat);
	}

	/ create the buffers that send information to the fragment shader and copy in the information
	for (auto& fragShaderBuffer : m_fragShaderBuffers) {
		fragShaderBuffer = fragShaderBuffer.create(sizeof(MaterialFragmentData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		fragShaderBuffer->copy_data(&fragDat);
	}
	*/
}

// material
Material::MaterialSystem::MaterialSystem(Camera* const camera, Material* const material) : m_camera(camera), m_material(material) {
	// create the descriptors themselves
	for (auto& descriptorSet : m_descriptorSets) {
		// get a unused descriptor set and push back its pointer
		descriptorSet = m_camera->m_renderPipeline.descriptorSystem(1).get_unused_set();
		// add the writes
		descriptorSet->add_writes({ // write the images
			{ m_material->normalMapTexture.texture->get_descriptor_image_info(), 3, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER},
			{ m_material->displacementMapTexture.texture->get_descriptor_image_info(), 4, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER},
			{ m_material->albedoTexture.texture->get_descriptor_image_info(), 2, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER},
			{ m_material->metallicTexture.texture->get_descriptor_image_info(), 6, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER},
			{ m_material->emissionTexture.texture->get_descriptor_image_info(), 7, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER},
			{ m_material->occlusionMapTexture.texture->get_descriptor_image_info(), 8, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER},
		});
		descriptorSet->add_writes({ // write the buffers
			{ m_material->m_vertShaderBuffers[0]->get_descriptor_buffer_info(), 1, lyra::vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_UNIFORM_BUFFER },
			{ m_material->m_vertShaderBuffers[1]->get_descriptor_buffer_info(), 1, lyra::vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_UNIFORM_BUFFER },
			{ m_material->m_fragShaderBuffers[0]->get_descriptor_buffer_info(), 5, lyra::vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_UNIFORM_BUFFER },
			{ m_material->m_fragShaderBuffers[1]->get_descriptor_buffer_info(), 5, lyra::vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_UNIFORM_BUFFER }
		});
		// update the descriptor set
		descriptorSet->update();
	}

	/// @todo use a deque with function pointers for this, probably better
	// m_camera->m_materials.push_back(this);
}

void Material::MaterialSystem::draw() const {
	// bind the descriptor set first
	Application::renderSystem.frames[Application::renderSystem.currentFrame()].commandBuffer().bindDescriptorSet(
		m_camera->m_renderPipeline.bindPoint(), 
		m_camera->m_renderPipeline.layout(), 
		1, 
		*m_descriptorSets[Application::renderSystem.currentFrame()]
	);
	// draw all the meshes
	for (const auto& meshRenderer : m_meshRenderers) meshRenderer->draw();
}

} // namespace lyra
