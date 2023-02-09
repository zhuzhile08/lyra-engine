/*************************
 * @file material.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a material system
 *
 * @date 2022-05-27
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <lyra.h>

#include <core/array.h>
#include <core/smart_pointer.h>

#include <nodes/node.h>

#include <rendering/vulkan/GPU_buffer.h>
#include <rendering/vulkan/descriptor.h>
#include <rendering/assets.h>

namespace lyra {

class Material {
public:
	/**
	 * @brief Construct a new Material object
	 * 
	 * @param renderer renderer the material is drawn uses
	 * @param meshRenderers mesh renderers with the material
	 * @param albedoColor color for the default
	 * @param albedoTexture texture for the default
	 * @param metallic metallic value
	 * @param roughness roughness value
	 * @param specular specular value
	 * @param metallicTexture metallic texture
	 * @param emissionEnabled texture light emission
	 * @param emissionColor light color if enabled
	 * @param emissionTexture light texture if enabled
	 * @param emissionEnergy light energy value if enabled
	 * @param normalMapTexture normal map texture
	 * @param normalMapValue normal map strength
	 * @param displacementMapTexture height map texture
	 * @param displacementMapValue height map strength
	 * @param occlusionMapTexture occlusion map texture
	 * @param occlusionMapValue occlusion map value
	 */
	Material(
		Camera* const camera,
		const std::vector<MeshRenderer*>& meshRenderers,
		const Color& albedoColor = Color(),
		const Texture* const albedoTexture = nullptr,
		const uint8& metallic = 0,
		const uint8& roughness = 0,
		const uint8& specular = 0,
		const Texture* const metallicTexture = nullptr,
		const bool& emissionEnabled = false,
		const Color& emissionColor = Color(),
		const Texture* const emissionTexture = nullptr,
		const uint8& emissionEnergy = 0,
		const Texture* const normalMapTexture = nullptr,
		const int8& normalMapValue = 0,
		const Texture* const displacementMapTexture = nullptr,
		const uint8& displacementMapValue = 0,
		const Texture* const occlusionMapTexture = nullptr,
		const uint8& occlusionMapValue = 0
	);
	/**
	 * @brief destroy the material
	 */
	~Material() { 
		for (uint32 i = 0; i < m_descriptorSets.size(); i++) m_descriptorSets[i]->recycle(); 
	}

	/**
	 * @brief copy a material
	 * 
	 * @param other the other material
	 * @return lyra::Material& 
	 */
	// Material& operator=(const Material& other);
	/**
	 * @brief copy a material
	 * 
	 * @param other the other material
	 * @return Material& 
	 */
	// Material& operator=(Material&& other);

	/**
	 * @brief get the descriptor sets
	 *
	 * @return const Array<vulkan::DescriptorSystem::DescriptorSet*, Settings::RenderConfig::maxFramesInFlight>&
	*/
	NODISCARD const Array<vulkan::DescriptorSystem::DescriptorSet*, Settings::RenderConfig::maxFramesInFlight>& descriptorSets() const noexcept { return m_descriptorSets; }

	Color albedoColor;
	const Texture* albedoTexture;

	uint8 metallic;
	uint8 roughness;
	uint8 specular;
	const Texture* metallicTexture;

	bool emissionEnabled;
	Color emissionColor;
	const Texture* emissionTexture;
	uint8 emissionEnergy;
	
	const Texture* normalMapTexture;
	int8 normalMapValue;

	const Texture* displacementMapTexture;
	uint8 displacementMapValue;

	const Texture* occlusionMapTexture;
	uint8 occlusionMapValue;
	

private:
	Camera* const camera;

	std::vector<MeshRenderer*> m_meshRenderers;

	Array<vulkan::DescriptorSystem::DescriptorSet*, Settings::RenderConfig::maxFramesInFlight> m_descriptorSets;
	Array<SmartPointer<vulkan::GPUBuffer>, Settings::RenderConfig::maxFramesInFlight> m_fragShaderBuffers;
	Array<SmartPointer<vulkan::GPUBuffer>, Settings::RenderConfig::maxFramesInFlight> m_vertShaderBuffers;

	struct MaterialVertexData {
		alignas(8) const uint32 m_normalMapValue;
		alignas(8) const int32 m_displacementMapValue;
	};

	struct MaterialFragmentData {
		alignas(16) const glm::vec4 m_albedoColor;
		alignas(16) const glm::vec4 m_emissionColor;
		alignas(8) const uint32 m_metallic;
		alignas(8) const uint32 m_roughness;
		alignas(8) const uint32 m_specular;
		alignas(8) const bool m_emissionEnabled;
		alignas(8) const uint32 m_emissionEnergy;
		alignas(8) const uint32 m_occlusionMapValue;
	};

	/**
	 * @brief draw all meshes using the material
	 */
	void draw() const;

	friend class Renderer;
	friend class Camera;
};

} // namespace lyra