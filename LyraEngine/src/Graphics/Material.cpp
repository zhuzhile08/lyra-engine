#include <Graphics/Material.h>

#include <Graphics/Texture.h>

#include <Resource/ResourceSystem.h>

namespace lyra {

// material data
Material::Material(
	const Color& albedoColor,
	const lsd::Vector<const Texture*>& albedoTextures,
	float32 metallic,
	float32 roughness,
	const Texture* metallicTexture,
	const Color& specularColor,
	const Texture* specularTexture,
	const Color& emissionColor,
	const Texture* emissionTexture,
	const Texture* normalMapTexture,
	const Texture* displacementMapTexture,
	const Color& occlusionColor,
	const Texture* occlusionMapTexture,
	const vulkan::GraphicsPipeline::Builder& pipelineBuilder,
	const vulkan::GraphicsProgram::Builder& programBuilder
) : m_graphicsPipeline(&renderer::graphicsPipeline(pipelineBuilder, programBuilder)),
	m_descriptorSets(*m_graphicsPipeline->program, 0, true),
	m_albedoColor(albedoColor),
	m_albedoTextures(albedoTextures),
	m_metallic(metallic),
	m_roughness(roughness),
	m_metallicTexture(metallicTexture),
	m_specularColor(specularColor),
	m_specularTexture(specularTexture),
	m_emissionColor(emissionColor),
	m_emissionTexture(emissionTexture),
	m_normalMapTexture(normalMapTexture),
	m_displacementMapTexture(displacementMapTexture),
	m_occlusionColor(occlusionColor),
	m_occlusionMapTexture(occlusionMapTexture)
{
	// uniform data to send to the fragment shader
	FragmentShaderData fragDat {
		m_albedoColor,
		m_emissionColor,
		m_specularColor,
		m_occlusionColor,
		m_metallic,
		m_roughness
	};

	for (uint32 i = 0; i < config::maxFramesInFlight; i++) {
		m_fragShaderBuffers[i] = vulkan::GPUBuffer(sizeof(FragmentShaderData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		m_fragShaderBuffers[i].copyData(&fragDat);

		m_descriptorSets.addWrites({
			{ { m_fragShaderBuffers[i].getDescriptorBufferInfo() }, 6, lyra::vulkan::DescriptorSets::Type::uniformBuffer}
		});
	}

	lsd::Vector<VkDescriptorImageInfo> albedoImageInfos(m_albedoTextures.size());

	for (uint32 i = 0; i < m_albedoTextures.size(); i++) {
		albedoImageInfos[i] = m_albedoTextures[i]->getDescriptorImageInfo();
	}

	m_descriptorSets.addWrites({
		{ 
			{ m_normalMapTexture ? m_normalMapTexture->getDescriptorImageInfo() : resource::defaultNormal().getDescriptorImageInfo() }, 
			0, 
			lyra::vulkan::DescriptorSets::Type::imageSampler
		},
		{ 
			{ m_specularTexture ? m_specularTexture->getDescriptorImageInfo() : resource::defaultTexture().getDescriptorImageInfo() }, 
			1, 
			lyra::vulkan::DescriptorSets::Type::imageSampler
		},
		{ 
			{ m_displacementMapTexture ? m_displacementMapTexture->getDescriptorImageInfo() : resource::defaultTexture().getDescriptorImageInfo() }, 
			2, 
			lyra::vulkan::DescriptorSets::Type::imageSampler
		},
		{ 
			{ m_metallicTexture ? m_metallicTexture->getDescriptorImageInfo() : resource::defaultTexture().getDescriptorImageInfo() }, 
			3, 
			lyra::vulkan::DescriptorSets::Type::imageSampler
		},
		{ 
			{ m_emissionTexture ? m_emissionTexture->getDescriptorImageInfo() : resource::defaultTexture().getDescriptorImageInfo() }, 
			4, 
			lyra::vulkan::DescriptorSets::Type::imageSampler
		},
		{ 
			{ m_occlusionMapTexture ? m_occlusionMapTexture->getDescriptorImageInfo() : resource::defaultTexture().getDescriptorImageInfo() }, 
			5, 
			lyra::vulkan::DescriptorSets::Type::imageSampler
		},
		{ 
			albedoImageInfos.empty() ? lsd::Vector<VkDescriptorImageInfo> { resource::defaultTexture().getDescriptorImageInfo() } : albedoImageInfos, 
			7, 
			lyra::vulkan::DescriptorSets::Type::imageSampler
		}
	});
}

} // namespace lyra
