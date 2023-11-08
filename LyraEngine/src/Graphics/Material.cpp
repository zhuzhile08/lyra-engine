#include <Graphics/Material.h>

#include <Graphics/Texture.h>

#include <Resource/ResourceSystem.h>

namespace lyra {

// material data
Material::Material(
	const Color& albedoColor,
	const std::vector<const Texture*>& albedoTextures,
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
	const Texture* occlusionMapTexture
) : m_albedoColor(albedoColor),
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
			{ { m_fragShaderBuffers[i].getDescriptorBufferInfo() }, 5, lyra::vulkan::DescriptorSets::Type::uniformBuffer}
		});
	}

	std::vector<VkDescriptorImageInfo> albedoImageInfos(m_albedoTextures.size());

	for (const auto& t : m_albedoTextures) {
		albedoImageInfos.push_back(t->getDescriptorImageInfo());
	}

	m_descriptorSets.addWrites({
		{ { m_normalMapTexture->getDescriptorImageInfo() }, 0, lyra::vulkan::DescriptorSets::Type::imageSampler},
		{ { m_displacementMapTexture->getDescriptorImageInfo() }, 1, lyra::vulkan::DescriptorSets::Type::imageSampler},
		{ { m_metallicTexture->getDescriptorImageInfo() }, 2, lyra::vulkan::DescriptorSets::Type::imageSampler},
		{ { m_emissionTexture->getDescriptorImageInfo() }, 3, lyra::vulkan::DescriptorSets::Type::imageSampler},
		{ { m_occlusionMapTexture->getDescriptorImageInfo() }, 4, lyra::vulkan::DescriptorSets::Type::imageSampler},
		{ albedoImageInfos, 6, lyra::vulkan::DescriptorSets::Type::imageSampler}
	});
}

} // namespace lyra
