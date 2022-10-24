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

#include <nodes/node.h>

#include <rendering/vulkan/GPU_buffer.h>
#include <rendering/vulkan/descriptor.h>

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
	 * @param heightMapTexture height map texture
	 * @param heightMapValue height map strength
	 * @param occlusionMapTexture occlusion map texture
	 * @param occlusionMapValue occlusion map value
	 */
	Material(
		const Camera* const camera,
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
		const Texture* const heightMapTexture = nullptr,
		const uint8& heightMapValue = 0,
		const Texture* const occlusionMapTexture = nullptr,
		const uint8& occlusionMapValue = 0
	);

	/**
	 * @brief get the descriptor sets
	 *
	 * @return const std::vector<vulkan::Descriptor>&
	*/
	NODISCARD const std::vector<vulkan::Descriptor>& descriptor() const noexcept { return m_descriptors; }

private:
	Color m_albedoColor;
	const Texture* m_albedoTexture;

	uint8 m_metallic;
	uint8 m_roughness;
	uint8 m_specular;
	const Texture* m_metallicTexture;

	bool m_emissionEnabled;
	Color m_emissionColor;
	const Texture* m_emissionTexture;
	uint8 m_emissionEnergy;
	
	const Texture* m_normalMapTexture;
	int8 m_normalMapValue;

	const Texture* m_heightMapTexture;
	uint8 m_heightMapValue;

	const Texture* m_occlusionMapTexture;
	uint8 m_occlusionMapValue;

	std::vector<MeshRenderer*> m_meshRenderers;

	std::vector<vulkan::Descriptor> m_descriptors;
	std::vector<vulkan::GPUBuffer> m_fragShaderBuffers;
	std::vector<vulkan::GPUBuffer> m_vertShaderBuffers;

	const Camera* camera;

	struct MaterialVertexData {
		uint32 m_normalMapValue;
		int32 m_heightMapValue;
	};

	struct MaterialFragmentData {
		Color m_albedoColor;
		uint32 m_metallic;
		uint32 m_roughness;
		uint32 m_specular;
		bool m_emissionEnabled;
		Color m_emissionColor;
		uint32 m_emissionEnergy;
		uint32 m_occlusionMapValue;
	};

	/**
	 * @brief draw all meshes using the material
	 */
	void draw() const;

	friend class Renderer;
	friend class Camera;
};

} // namespace lyra