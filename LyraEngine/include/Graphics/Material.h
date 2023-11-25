/*************************
 * @file Material.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a material system
 *
 * @date 2022-05-27
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/Array.h>

#include <Resource/LoadMaterialFile.h>

#include <Graphics/Renderer.h>
#include <Graphics/VulkanRenderSystem.h>

namespace lyra {

class Material {
public:
	struct FragmentShaderData {
		alignas(16) Color albedoColor;
		alignas(16) Color emissionColor;
		alignas(16) Color specularColor;
		alignas(16) Color occlusionColor;
		alignas(8) float metallic;
		alignas(8) float roughness;
	};

	Material(
		const Color& albedoColor = Color(),
		const std::vector<const Texture*>& albedoTextures = { },
		float32 metallic = 0.0f,
		float32 roughness = 0.0f,
		const Texture* metallicTexture = nullptr,
		const Color& specularColor = Color(),
		const Texture* specularTexture = nullptr,
		const Color& emissionColor = Color(),
		const Texture* emissionTexture = nullptr,
		const Texture* normalMapTexture = nullptr,
		const Texture* displacementMapTexture = nullptr,
		const Color& occlusionColor = Color(),
		const Texture* occlusionMapTexture = nullptr,
		const vulkan::GraphicsPipeline::Builder& pipelineBuilder = { },
		const vulkan::GraphicsProgram::Builder& programBuilder = { }
	);

private:
	Array<vulkan::GPUBuffer, config::maxFramesInFlight> m_fragShaderBuffers;
	Array<vulkan::GPUBuffer, config::maxFramesInFlight> m_vertShaderBuffers;

	vulkan::GraphicsPipeline* m_graphicsPipeline = nullptr;

	vulkan::DescriptorSets m_descriptorSets;

	Color m_albedoColor;
	std::vector<const Texture*> m_albedoTextures;

	float32 m_metallic;
	float32 m_roughness;
	const Texture* m_metallicTexture;

	Color m_specularColor;
	const Texture* m_specularTexture;

	Color m_emissionColor;
	const Texture* m_emissionTexture;
	
	const Texture* m_normalMapTexture;

	const Texture* m_displacementMapTexture;

	Color m_occlusionColor;
	const Texture* m_occlusionMapTexture;

	friend void renderer::draw();
};

} // namespace lyra
